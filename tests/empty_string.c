// SPDX-License-Identifer: MIT

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sjp.h"


static void sjp_cb(sjp_t *sjp);

int main(int argc, char *argv[])
{
    const char *p, buf[] = "[\"\"]";
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
        assert((e->t & SJP_TYPE) == SJP_ARRAY_T);
        assert((e->t & SJP_FLAGS) == SJP_START);
        break;

    case 1:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == (SJP_START | SJP_END));
        assert(sjp->str != NULL);
        assert(sjp->str_bytes == 0);
        break;

    case 2:
        assert(sjp->d == 0);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_ARRAY_T);
        assert((e->t & SJP_FLAGS) == SJP_END);
        break;

    default:
        assert(0);
        break;
    }
}
