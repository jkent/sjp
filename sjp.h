// SPDX-License-Identifer: MIT

#pragma once


#ifndef SJP_STK_SIZE
# define SJP_STK_SIZE 8
#endif

#ifndef SJP_BUF_SIZE
# define SJP_BUF_SIZE 32
#endif

typedef enum sjp_token sjp_token_t;
typedef struct sjp sjp_t;
typedef struct sjp_item sjp_item_t;
typedef void (*sjp_cb_t)(sjp_t *sjp);

enum sjp_token {
    SJP_NONE_T      = 0,
    SJP_OBJ_T,
    SJP_ARRAY_T,
    SJP_STR_T,
    SJP_NUM_T,
    SJP_TRUE_T,
    SJP_FALSE_T,
    SJP_NULL_T,
    SJP_TYPE        = 7,
    SJP_KEY         = 16,
    SJP_START       = 32,
    SJP_END         = 64,
    SJP_FLAGS       = 240,
    SJP_COMMA       = 256,
    SJP_COLON       = 512,
};

struct sjp_item {
    unsigned int i;
    sjp_token_t t;
};

#ifndef SJP_PRIVATE_STRUCTS
struct sjp {
    /// public members
    void *user;
    sjp_cb_t *cb;
    unsigned int d;
    sjp_item_t stk[SJP_STK_SIZE];
    union {
        struct {
            const char *str;
            unsigned int str_bytes;
        };
        double num;
    };
};
#endif

sjp_t *sjp_init(sjp_cb_t cb);
void sjp_free(sjp_t *sjp);
void sjp_reset(sjp_t *sjp);
int sjp_parse(sjp_t *sjp, const char *buf, unsigned int len);
