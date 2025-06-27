// SPDX-License-Identifer: MIT

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sjp.h"


static void sjp_cb(sjp_t *sjp);

int main(int argc, char *argv[])
{
    const char *p, buf[] = "{\"a\":true,\"b\":123,\"c\":\"value\"}";
    unsigned int len;
    int ret;
    sjp_t *sjp;

    sjp = sjp_init(sjp_cb);
    assert(sjp != NULL);

    p = buf;
    len = strlen(buf);
    while ((ret = sjp_parse(sjp, p, len)) > 0) {
        p += ret;
        len -= ret;
    }

    assert(ret >= 0);

    sjp_free(sjp);

    exit(EXIT_SUCCESS);
    return 0;
}

static void sjp_cb(sjp_t *sjp)
{
    sjp_item_t *e = &sjp->stk[sjp->d];
    static unsigned int n = 0;

    fprintf(stderr, "n=%u d=%u e->t=%u e->i=%u\n", n, sjp->d, e->t, e->i);

    switch (n++) {
    case 0:
        assert(sjp->d == 0);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_OBJ_T);
        assert((e->t & SJP_FLAGS) == SJP_START);
        break;

    case 1:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == (SJP_KEY | SJP_START | SJP_END));
        assert(sjp->str_bytes == 1);
        assert(memcmp(sjp->str, "a", sjp->str_bytes) == 0);
        break;

    case 2:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_TRUE_T);
        assert((e->t & SJP_FLAGS) == (SJP_START | SJP_END));
        break;

    case 3:
        assert(sjp->d == 1);
        assert(e->i == 1);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == (SJP_KEY | SJP_START | SJP_END));
        assert(sjp->str_bytes == 1);
        assert(memcmp(sjp->str, "b", sjp->str_bytes) == 0);
        break;

    case 4:
        assert(sjp->d == 1);
        assert(e->i == 1);
        assert((e->t & SJP_TYPE) == SJP_NUM_T);
        assert((e->t & SJP_FLAGS) == (SJP_START | SJP_END));
        assert(sjp->num == 123);
        break;

    case 5:
        assert(sjp->d == 1);
        assert(e->i == 2);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == (SJP_KEY | SJP_START | SJP_END));
        assert(sjp->str_bytes == 1);
        assert(memcmp(sjp->str, "c", sjp->str_bytes) == 0);
        break;

    case 6:
        assert(sjp->d == 1);
        assert(e->i == 2);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == (SJP_START | SJP_END));
        assert(sjp->str_bytes == 5);
        assert(memcmp(sjp->str, "value", sjp->str_bytes) == 0);
        break;

    case 7:
        assert(sjp->d == 0);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_OBJ_T);
        assert((e->t & SJP_FLAGS) == SJP_END);
        break;

    default:
        assert(0);
        break;
    }
}
