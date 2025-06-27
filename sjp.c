// SPDX-License-Identifer: MIT

#include <ctype.h>
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
        };
        double num;
    };

    /// private members
    unsigned int temp_bytes;
    char temp[8];
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

static inline int sjp_utf8_cp_size(char c)
{
    if ((c & 0x80) == 0x00) {        /* 1-c ASCII */
        return 1;
    } else if ((c & 0xC0) == 0x80) { /* Continuation */
        return 0;
    } else if ((c & 0xE0) == 0xC0) { /* 2-c */
        return 2;
    } else if ((c & 0xF0) == 0xE0) { /* 3-c */
        return 3;
    } else if ((c & 0xF8) == 0xF0) { /* 4-c */
        return 4;
    }
    return -1;
}

static inline int parse_string_byte(sjp_t *sjp, char c)
{
    sjp_item_t *item = &sjp->stk[sjp->d];

    if (sjp->temp_bytes == 0) {
        if (c == '\\') {
            sjp->temp[sjp->temp_bytes++] = c;
            return 0;
        } else if (c == '\"') {
            item->t |= SJP_END;
            sjp->str = sjp->buf;
            sjp->str_bytes = sjp->buflen;
            return 1;
        } else {
            int len = sjp_utf8_cp_size(c);
            if (len <= 0) { /* invalid UTF8 */
                return -1;
            }
            if (SJP_BUF_SIZE - sjp->buflen < len) { /* make room for cp */
                sjp->str = sjp->buf;
                sjp->str_bytes = sjp->buflen;
                return 2;
            }
            if (len == 1) { /* If ASCII, bypass stashing */
                sjp->buf[sjp->buflen++] = c;
            } else { /* If cp with 2 or more bytes, stash first c */
                sjp->temp[sjp->temp_bytes++] = c;
            }
            return 0;
        }
    } else {
        int len = sjp_utf8_cp_size(sjp->temp[0]);
        if (len == 1) { /* if ASCII, must be escape */
            if (sjp->temp_bytes == 1) {
                if (SJP_BUF_SIZE - sjp->buflen < 1) {
                    sjp->str = sjp->buf;
                    sjp->str_bytes = sjp->buflen;
                    return 2;
                }
                switch (c) {
                case '\"':
                case '\\':
                case '/':
                    sjp->buf[sjp->buflen++] = c;
                    break;
                case 'b':
                    sjp->buf[sjp->buflen++] = '\b';
                    break;
                case 'f':
                    sjp->buf[sjp->buflen++] = '\f';
                    break;
                case 'n':
                    sjp->buf[sjp->buflen++] = '\n';
                    break;
                case 'r':
                    sjp->buf[sjp->buflen++] = '\r';
                    break;
                case 't':
                    sjp->buf[sjp->buflen++] = '\t';
                    break;
                case 'u':
                    sjp->temp[sjp->temp_bytes++] = c;
                    return 0;
                default:
                    return -1;
                }
                sjp->temp_bytes = 0;
                return 0;
            } else if (sjp->temp_bytes < 5) {
                sjp->temp[sjp->temp_bytes++] = c;
                return 0;
            } else {
                sjp->temp[sjp->temp_bytes] = c;
                unsigned int utf16 = strtol(&sjp->temp[2], NULL, 16);
                if (utf16 < 128) {
                    sjp->buf[sjp->buflen++] = utf16;
                } else if (utf16 < 2048) {
                    if (SJP_BUF_SIZE - sjp->buflen < 1) {
                        sjp->str = sjp->buf;
                        sjp->str_bytes = sjp->buflen;
                        return 2;
                    }
                    sjp->buf[sjp->buflen++] = 0xc0 | (utf16 >> 6);
                    sjp->buf[sjp->buflen++] = 0x80 | (utf16 & 0x3f);
                } else {
                    if (SJP_BUF_SIZE - sjp->buflen < 2) {
                        sjp->str = sjp->buf;
                        sjp->str_bytes = sjp->buflen;
                        return 2;
                    }
                    sjp->buf[sjp->buflen++] = 0xe0 | (utf16 >> 12);
                    sjp->buf[sjp->buflen++] = 0x80 | ((utf16 >> 6) & 0x3f);
                    sjp->buf[sjp->buflen++] = 0x80 | (utf16 & 0x3f);
                }
                sjp->temp_bytes = 0;
                return 0;
            }
        } else { /* must be a codepoint */
            sjp->temp[sjp->temp_bytes++] = c;
            if (len == sjp->temp_bytes) {
                memcpy(&sjp->buf[sjp->buflen], sjp->temp, len);
                sjp->temp_bytes = 0;
            }
            return 0;
        }
    }
    return -1;
}

static inline int parse_number_byte(sjp_t *sjp, char c)
{
    if (sjp->buflen >= SJP_BUF_SIZE - 1) {
        return -1;
    }

    if (isdigit(c) || c == '.' || c == '+' || c == '-' || c == 'E' || c == 'e') {
        sjp->buf[sjp->buflen++] = c;
        return 0;
    }

    sjp->buf[sjp->buflen++] = '\0';
    sjp->num = strtod(sjp->buf, NULL);
    return 2;
}

static inline int parse_literal_byte(sjp_t *sjp, char c)
{
    sjp_item_t *item = &sjp->stk[sjp->d];

    switch (item->t & SJP_TYPE) {
    case SJP_TRUE_T:
        switch (sjp->temp_bytes++) {
        case 1: return (c == 'r') ? 0 : -1;
        case 2: return (c == 'u') ? 0 : -1;
        case 3: return (c == 'e') ? 1 : -1;
        default: return -1;
        }

    case SJP_FALSE_T:
        switch (sjp->temp_bytes++) {
        case 1: return (c == 'a') ? 0 : -1;
        case 2: return (c == 'l') ? 0 : -1;
        case 3: return (c == 's') ? 0 : -1;
        case 4: return (c == 'e') ? 1 : -1;
        default: return -1;
        }

    case SJP_NULL_T:
        switch (sjp->temp_bytes++) {
        case 1: return (c == 'u') ? 0 : -1;
        case 2: return (c == 'l') ? 0 : -1;
        case 3: return (c == 'l') ? 1 : -1;
        default: return -1;
        }
    }
    return -1;
}

static inline int parse_byte(sjp_t *sjp, char c)
{
    sjp_item_t *parent = &sjp->stk[sjp->d - 1];
    sjp_item_t *item = &sjp->stk[sjp->d];
    sjp_item_t *child = &sjp->stk[sjp->d + 1];

    switch (item->t & SJP_TYPE) {
    case SJP_NONE_T:

        item->t &= ~SJP_END;

        switch (c) {
        case ' ':
        case '\n':
        case '\r':
        case '\t':
            return 0;

        case ',':
            if (sjp->d != 0 && !(item->t & SJP_COMMA)) {
                return -1;
            }
            if (sjp->d > 0 && (parent->t & SJP_TYPE) == SJP_OBJ_T) {
                item->t = SJP_KEY;
                return 0;
            }
            item->t = SJP_NONE_T;
            return 0;

        case ':':
            if (!(item->t & SJP_COLON)) {
                return -1;
            }
            item->t = SJP_NONE_T;
            return 0;
        }

        if (sjp->d > 0) {
            switch (c) {
            case '}':
                if ((parent->t & SJP_TYPE) != SJP_OBJ_T) {
                    return -1;
                }
                if (item->i != 0 && (item->t & SJP_KEY)) {
                    return -1;
                }
                parent->t = SJP_OBJ_T | SJP_END;
                sjp->d--;
                return 1;

            case ']':
                if ((parent->t & SJP_TYPE) != SJP_ARRAY_T) {
                    return -1;
                }
                parent->t = SJP_ARRAY_T | SJP_END;
                sjp->d--;
                return 1;
            }
        }

        if (sjp->d >= SJP_STK_SIZE - 1) {
            return -1;
        }

        if (!(item->t & SJP_KEY)) {
            switch (c) {
            case '{':
                item->t = SJP_OBJ_T | SJP_START;
                child->t = SJP_KEY;
                child->i = 0;
                return 1;
            case '[':
                item->t = SJP_ARRAY_T | SJP_START;
                child->t = SJP_NONE_T;
                child->i = 0;
                return 1;
            }
        }

        if (sjp->d == 0) {
            return -1;
        }

        if (c == '"') {
            item->t |= SJP_STR_T | SJP_START;
            return 0;
        }

        if (item->t & SJP_KEY) {
            return -1;
        }

        switch (c) {
        case '-':
        case '0' ... '9':
            item->t |= SJP_NUM_T | SJP_START | SJP_END;
            sjp->buf[sjp->buflen++] = c;
            return 0;
        case 't':
            item->t = SJP_TRUE_T | SJP_START | SJP_END;
            sjp->temp_bytes++;
            return 0;
        case 'f':
            item->t = SJP_FALSE_T | SJP_START | SJP_END;
            sjp->temp_bytes++;
            return 0;
        case 'n':
            item->t = SJP_NULL_T | SJP_START | SJP_END;
            sjp->temp_bytes++;
            return 0;
        }
        return -1;

    case SJP_STR_T:
        return parse_string_byte(sjp, c);

    case SJP_NUM_T:
        return parse_number_byte(sjp, c);

    case SJP_TRUE_T:
    case SJP_FALSE_T:
    case SJP_NULL_T:
        return parse_literal_byte(sjp, c);
    }
    return -1;
}

int sjp_parse(sjp_t *sjp, const char *buf, unsigned int len)
{
    const char *p = buf;

    while (p < buf + len) {
        char c = *p++;
        int ret = parse_byte(sjp, c);
        if (ret < 0) {
            return -1;
        }
        if (ret > 0) {
            sjp_item_t *item = &sjp->stk[sjp->d];

            sjp->cb(sjp);
            if (c == '{' || c == '[') {
                sjp->d++;
                continue;
            }

            sjp->buflen = 0;
            sjp->temp_bytes = 0;
            item->t &= ~SJP_START;
            if (item->t & SJP_END) {
                sjp->stk[sjp->d].t &= ~SJP_TYPE;
                if (item->t & SJP_KEY) {
                    item->t = SJP_COLON;
                } else {
                    item->t = SJP_COMMA;
                    item->i++;
                }
            }
        }
        if (ret > 1) {
            p--;
        }
    }

    return p - buf;
}
