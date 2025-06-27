#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "sjp.h"


typedef struct string {
    const char *str;
    unsigned int len;
} string_t;

static const char *document;
static const string_t strings[];
static void cb(sjp_t *sjp);

#define DEBUG_CB() fprintf(stderr, "n=%u d=%u stk[d].t=%u stk[d].i=%u\n", n, \
        sjp->d, sjp->stk[sjp->d].t, sjp->stk[sjp->d].i)

#define ASSERT_OBJ_START(depth, idx) ({ \
    assert(sjp->d == depth); \
    assert(sjp->stk[sjp->d].i == idx); \
    assert((sjp->stk[sjp->d].t & SJP_TYPE) == SJP_OBJ_T); \
    assert((sjp->stk[sjp->d].t & SJP_FLAGS) == SJP_START); \
})

#define ASSERT_OBJ_END(depth, idx) ({ \
    assert(sjp->d == depth); \
    assert(sjp->stk[sjp->d].i == idx); \
    assert((sjp->stk[sjp->d].t & SJP_TYPE) == SJP_OBJ_T); \
    assert((sjp->stk[sjp->d].t & SJP_FLAGS) == SJP_END); \
})

#define ASSERT_ARRAY_START(depth, idx) ({ \
    assert(sjp->d == depth); \
    assert(sjp->stk[sjp->d].i == idx); \
    assert((sjp->stk[sjp->d].t & SJP_TYPE) == SJP_ARRAY_T); \
    assert((sjp->stk[sjp->d].t & SJP_FLAGS) == SJP_START); \
})

#define ASSERT_ARRAY_END(depth, idx) ({ \
    assert(sjp->d == depth); \
    assert(sjp->stk[sjp->d].i == idx); \
    assert((sjp->stk[sjp->d].t & SJP_TYPE) == SJP_ARRAY_T); \
    assert((sjp->stk[sjp->d].t & SJP_FLAGS) == SJP_END); \
})

#define ASSERT_STR() ({ \
    assert((sjp->stk[sjp->d].t & SJP_TYPE) == SJP_STR_T); \
    if (str_pos == 0) { \
        assert(sjp->stk[sjp->d].t & SJP_START); \
    } \
    assert(str_pos + sjp->str_bytes <= strings[str_idx].len); \
    assert(memcmp(sjp->str, &strings[str_idx].str[str_pos], sjp->str_bytes) == 0); \
    str_pos += sjp->str_bytes; \
    if (sjp->stk[sjp->d].t & SJP_END) { \
        assert(str_pos == strings[str_idx++].len); \
        str_pos = 0; \
    } else { \
        n -= 1; \
    } \
})

#define ASSERT_STR_KEY(depth, idx) ({ \
    assert(sjp->d == depth); \
    assert(sjp->stk[sjp->d].i == idx); \
    assert(sjp->stk[sjp->d].t & SJP_KEY); \
    ASSERT_STR(); \
})

#define ASSERT_STR_VALUE(depth, idx) ({ \
    assert(sjp->d == depth); \
    assert(sjp->stk[sjp->d].i == idx); \
    assert((sjp->stk[sjp->d].t & SJP_KEY) != SJP_KEY); \
    ASSERT_STR(); \
})

#define ASSERT_NUM(depth, idx, value) ({ \
    assert(sjp->d == depth); \
    assert(sjp->stk[sjp->d].i == idx); \
    assert((sjp->stk[sjp->d].t & SJP_TYPE) == SJP_NUM_T); \
    assert((sjp->stk[sjp->d].t & SJP_FLAGS) == (SJP_START | SJP_END)); \
    assert(sjp->num == value); \
})

#define ASSERT_TRUE(depth, idx) ({ \
    assert(sjp->d == depth); \
    assert(sjp->stk[sjp->d].i == idx); \
    assert((sjp->stk[sjp->d].t & SJP_TYPE) == SJP_TRUE_T); \
    assert((sjp->stk[sjp->d].t & SJP_FLAGS) == (SJP_START | SJP_END)); \
})

#define ASSERT_FALSE(depth, idx) ({ \
    assert(sjp->d == depth); \
    assert(sjp->stk[sjp->d].i == idx); \
    assert((sjp->stk[sjp->d].t & SJP_TYPE) == SJP_FALSE_T); \
    assert((sjp->stk[sjp->d].t & SJP_FLAGS) == (SJP_START | SJP_END)); \
})

#define ASSERT_NULL(depth, idx) ({ \
    assert(sjp->d == depth); \
    assert(sjp->stk[sjp->d].i == idx); \
    assert((sjp->stk[sjp->d].t & SJP_TYPE) == SJP_NULL_T); \
    assert((sjp->stk[sjp->d].t & SJP_FLAGS) == (SJP_START | SJP_END)); \
})
