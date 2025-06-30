// SPDX-License-Identifer: MIT

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SJP_PRIVATE_STRUCTS
#include "sjp.h"


struct sjp {
    /// public members
    void *user;
    sjp_cb_t cb;
    unsigned int d;
    sjp_item_t stk[SJP_STK_SIZE];
    union {
        struct {
            const char *str;
            unsigned int str_bytes;
            unsigned int str_pos;
        };
        double num;
    };

    /// private members
    unsigned int buflen;
    char buf[SJP_BUF_SIZE];
};

sjp_t *sjp_init(sjp_cb_t cb)
{
    sjp_t *sjp;

    sjp = calloc(1, sizeof(sjp_t));
    sjp->cb = cb;

    return sjp;
}

void sjp_free(sjp_t *sjp)
{
    free(sjp);
}

void sjp_reset(sjp_t *sjp)
{
    void *user_temp = sjp->user;
    sjp_cb_t cb_temp = sjp->cb;
    memset(sjp, 0, sizeof(*sjp));
    sjp->user = user_temp;
    sjp->cb = cb_temp;
}

static inline int num_cp_bytes(char c)
{
    if ((c & 0x80) == 0x00) {           /* 1-byte ASCII */
        return 1;
    } else if ((c & 0xC0) == 0x80) {    /* Continuation */
        return 0;
    } else if ((c & 0xE0) == 0xC0) {    /* 2-byte */
        return 2;
    } else if ((c & 0xF0) == 0xE0) {    /* 3-byte */
        return 3;
    } else if ((c & 0xF8) == 0xF0) {    /* 4-byte */
        return 4;
    }
    return -1;
}

static inline void call_cb(sjp_t *sjp)
{
    sjp->cb(sjp);
    sjp->stk[sjp->d].t &= ~SJP_START;
    sjp->str_pos += sjp->str_bytes;
    sjp->buflen = 0;
    sjp->str_bytes = 0;
}

static inline int parse_none_bytes(sjp_t *sjp, const char **buf, unsigned int *len)
{
    sjp_item_t *parent = &sjp->stk[sjp->d - 1];
    sjp_item_t *item = &sjp->stk[sjp->d];
    const char *p;

    if (*len == 0) {
        return -1;
    }

    while (*len) {
        (*len)--;
        p = (*buf)++;

        switch (*p) {
        case ' ':
        case '\n':
        case '\r':
        case '\t':
            continue;

        case ',':
            if (!(item->t & SJP_COMMA)) {
                return -1;
            }
            if (sjp->d > 0 && (parent->t & SJP_TYPE) == SJP_OBJ_T) {
                item->t = SJP_NONE_T | SJP_KEY;
                continue;
            }
            item->t = SJP_NONE_T;
            continue;

        case ':':
            if (!(item->t & SJP_COLON)) {
                return -1;
            }
            item->t = SJP_NONE_T;
            continue;
        }

        if (sjp->d > 0) {
            switch (*p) {
            case '}':
                if ((parent->t & SJP_TYPE) != SJP_OBJ_T) {
                    return -1;
                }
                if (item->i != 0 && (item->t & SJP_KEY)) {
                    return -1;
                }
                sjp->d--;
                parent = &sjp->stk[sjp->d - 1];
                item = &sjp->stk[sjp->d];
                item->t = SJP_OBJ_T | SJP_END;
                call_cb(sjp);
                if (item->t & SJP_KEY) {
                    item->t = SJP_NONE_T | SJP_COLON;
                } else {
                    item->i++;
                    item->t = SJP_NONE_T | SJP_COMMA;
                }
                continue;

            case ']':
                if ((parent->t & SJP_TYPE) != SJP_ARRAY_T) {
                    return -1;
                }
                sjp->d--;
                parent = &sjp->stk[sjp->d - 1];
                item = &sjp->stk[sjp->d];
                item->t = SJP_ARRAY_T | SJP_END;
                call_cb(sjp);
                item->i++;
                item->t = SJP_NONE_T | SJP_COMMA;
                continue;
            }
        }

        if (sjp->d >= SJP_STK_SIZE - 1) {
            return -1;
        }

        if (sjp->d != 0 && item->t & SJP_COMMA) {
            return -1;
        }

        if (!(item->t & SJP_KEY)) {
            switch (*p) {
            case '{':
                item->t = SJP_OBJ_T | SJP_START;
                call_cb(sjp);
                sjp->d++;
                parent = &sjp->stk[sjp->d - 1];
                item = &sjp->stk[sjp->d];
                item->i = 0;
                item->t = SJP_NONE_T | SJP_KEY;
                continue;

            case '[':
                item->t = SJP_ARRAY_T | SJP_START;
                call_cb(sjp);
                sjp->d++;
                parent = &sjp->stk[sjp->d - 1];
                item = &sjp->stk[sjp->d];
                item->i = 0;
                item->t = SJP_NONE_T;
                continue;
            }
        }

        if (*p == '"') {
            item->t |= SJP_STR_T | SJP_START;
            sjp->str_pos = 0;
            return 0;
        }

        if (item->t & SJP_KEY) {
            return -1;
        }

        switch (*p) {
        case '-':
        case '0' ... '9':
            item->t |= SJP_NUM_T | SJP_START | SJP_END;
            sjp->buf[sjp->buflen++] = *p;
            return 0;

        case 't':
            item->t = SJP_TRUE_T | SJP_START | SJP_END;
            sjp->buflen++;
            return 0;

        case 'f':
            item->t = SJP_FALSE_T | SJP_START | SJP_END;
            sjp->buflen++;
            return 0;

        case 'n':
            item->t = SJP_NULL_T | SJP_START | SJP_END;
            sjp->buflen++;
            return 0;
        }

        return -1;
    }

    return 0;
}

static inline int parse_string_bytes(sjp_t *sjp, const char **buf, unsigned int *len)
{
    const char *p;
    unsigned int bytes;

    while (*len) {
        bytes = (*len)--;
        p = (*buf)++;

        if (sjp->buflen == 0) {
            if (*p == '\\') {
                if (sjp->str_bytes) {
                    call_cb(sjp);
                }
                sjp->buf[sjp->buflen++] = *p;
                continue;
            } else if (*p == '\"') {
                sjp->stk[sjp->d].t |= SJP_END;
                call_cb(sjp);
                if (sjp->stk[sjp->d].t & SJP_KEY) {
                    sjp->stk[sjp->d].t = SJP_COLON;
                } else {
                    sjp->stk[sjp->d].i++;
                    sjp->stk[sjp->d].t = SJP_COMMA;
                }
                return 0;
            }

            int cp_bytes = num_cp_bytes(*p);
            if (cp_bytes <= 0) {
                return -1;
            }

            if (bytes < cp_bytes) {
                if (sjp->str_bytes) {
                    call_cb(sjp);
                }
                sjp->buf[sjp->buflen++] = *p;
                continue;
            }
            if (sjp->str_bytes++ == 0) {
                sjp->str = p;
            }
            while (--cp_bytes) {
                sjp->str_bytes++;
                bytes = (*len)--;
                p = (*buf)++;
                if ((*p & 0xC0) != 0x80) {
                    return -1;
                }
            }
        } else {
            int cp_bytes = num_cp_bytes(*sjp->buf);
            if (cp_bytes == 1) { /* must be an escape sequence */
                if (sjp->buflen == 1) {
                    sjp->str = sjp->buf;
                    sjp->str_bytes = 1;
                    switch (*p) {
                    case '\"':
                    case '\\':
                    case '/':
                        *sjp->buf = *p;
                        break;
                    case 'b':
                        *sjp->buf = '\b';
                        break;
                    case 'f':
                        *sjp->buf = '\f';
                        break;
                    case 'n':
                        *sjp->buf = '\n';
                        break;
                    case 'r':
                        *sjp->buf = '\r';
                        break;
                    case 't':
                        *sjp->buf = '\t';
                        break;
                    case 'u':
                        sjp->buf[sjp->buflen++] = 'u';
                        continue;
                    default:
                        return -1;
                    }
                } else if (sjp->buflen < 6) {
                    if (!isxdigit(*p)) {
                        return -1;
                    }
                    sjp->buf[sjp->buflen++] = *p;
                    if (sjp->buflen < 6) {
                        continue;
                    }
                    sjp->buf[sjp->buflen] = '\0';
                    sjp->buflen = 0;
                    unsigned int utf16 = strtol(&sjp->buf[2], NULL, 16);
                    if (utf16 < 128) {
                        sjp->buf[sjp->buflen] = utf16;
                    } else if (utf16 < 2048) {
                        sjp->str_bytes = 2;
                        sjp->buf[sjp->buflen++] = 0xc0 | (utf16 >> 6);
                        sjp->buf[sjp->buflen] = 0x80 | (utf16 & 0x3f);
                    } else {
                        sjp->str_bytes = 3;
                        sjp->buf[sjp->buflen++] = 0xe0 | (utf16 >> 12);
                        sjp->buf[sjp->buflen++] = 0x80 | ((utf16 >> 6) & 0x3f);
                        sjp->buf[sjp->buflen] = 0x80 | (utf16 & 0x3f);
                    }
                }
            } else {
                if ((*p & 0xC0) != 0x80) {
                    return -1;
                }
                sjp->buf[sjp->buflen++] = *p;
                if (sjp->buflen < cp_bytes) {
                    continue;
                }

                sjp->str = sjp->buf;
                sjp->str_bytes = sjp->buflen;
            }

            if (sjp->str_bytes) {
                call_cb(sjp);
            }
        }
    }

    return 0;
}

static inline int parse_number_bytes(sjp_t *sjp, const char **buf, unsigned int *len)
{
    const char *p;

    while (*len) {
        if (sjp->buflen >= SJP_BUF_SIZE - 2) {
            return -1;
        }

        (*len)--;
        p = (*buf)++;

        if (isdigit(*p) || *p == '.' || *p == '+' || *p == '-' || *p == 'E' || *p == 'e') {
            sjp->buf[sjp->buflen++] = *p;
            continue;
        }

        sjp->buf[sjp->buflen] = '\0';
        (*len)++;
        (*buf)--;
        sjp->num = strtod(sjp->buf, NULL);
        call_cb(sjp);
        sjp->stk[sjp->d].i++;
        sjp->stk[sjp->d].t = SJP_COMMA;
        break;
    }

    return 0;
}

static inline int parse_true_bytes(sjp_t *sjp, const char **buf, unsigned int *len)
{
    const char *p;

    while (*len) {
        (*len)--;
        p = (*buf)++;
        switch (sjp->buflen++) {
        case 1: if (*p != 'r') return -1; continue;
        case 2: if (*p != 'u') return -1; continue;
        case 3: if (*p != 'e') return -1;
        }

        call_cb(sjp);
        sjp->stk[sjp->d].i++;
        sjp->stk[sjp->d].t = SJP_COMMA;
        break;
    }

    return 0;
}

static inline int parse_false_bytes(sjp_t *sjp, const char **buf, unsigned int *len)
{
    const char *p;

    while (*len) {
        (*len)--;
        p = (*buf)++;
        switch (sjp->buflen++) {
        case 1: if (*p != 'a') return -1; continue;
        case 2: if (*p != 'l') return -1; continue;
        case 3: if (*p != 's') return -1; continue;
        case 4: if (*p != 'e') return -1;
        }

        call_cb(sjp);
        sjp->stk[sjp->d].i++;
        sjp->stk[sjp->d].t = SJP_COMMA;
        break;
    }

    return 0;
}

static inline int parse_null_bytes(sjp_t *sjp, const char **buf, unsigned int *len)
{
    const char *p;

    while (*len) {
        (*len)--;
        p = (*buf)++;
        switch (sjp->buflen++) {
        case 1: if (*p != 'u') return -1; continue;
        case 2: if (*p != 'l') return -1; continue;
        case 3: if (*p != 'l') return -1;
        }

        call_cb(sjp);
        sjp->stk[sjp->d].i++;
        sjp->stk[sjp->d].t = SJP_COMMA;
        break;
    }

    return 0;
}

static inline int parse_bytes(sjp_t *sjp, const char **buf, unsigned int *len)
{
    switch (sjp->stk[sjp->d].t & SJP_TYPE) {
    case SJP_NONE_T: return parse_none_bytes(sjp, buf, len);
    case SJP_STR_T: return parse_string_bytes(sjp, buf, len);
    case SJP_NUM_T: return parse_number_bytes(sjp, buf, len);
    case SJP_TRUE_T: return parse_true_bytes(sjp, buf, len);
    case SJP_FALSE_T: return parse_false_bytes(sjp, buf, len);
    case SJP_NULL_T: return parse_null_bytes(sjp, buf, len);
    }

    return -1;
}

int sjp_parse(sjp_t *sjp, const char *buf, unsigned int len)
{
    while (len) {
        if (parse_bytes(sjp, &buf, &len) < 0) {
            return -1;
        }
    }

    return 0;
}
