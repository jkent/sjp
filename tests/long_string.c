// SPDX-License-Identifer: MIT

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sjp.h"


static void sjp_cb(sjp_t *sjp);

int main(int argc, char *argv[])
{
    const char *p, buf[] = "[\"Lorem ipsum dolor sit amet, consectetur "
                           "adipiscing elit, sed do eiusmod tempor incididunt "
                           "ut labore et dolore magna aliqua. Ut enim ad minim "
                           "veniam, quis nostrud exercitation ullamco laboris "
                           "nisi ut aliquip ex ea commodo consequat. Duis aute "
                           "irure dolor in reprehenderit in voluptate velit "
                           "esse cillum dolore eu fugiat nulla pariatur. "
                           "Excepteur sint occaecat cupidatat non proident, "
                           "sunt in culpa qui officia deserunt mollit anim id "
                           "est laborum.\"]";
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
        assert((e->t & SJP_FLAGS) == SJP_START);
        assert(sjp->str_bytes == 32);
        assert(memcmp(sjp->str, "Lorem ipsum dolor sit amet, cons", sjp->str_bytes) == 0);
        break;

    case 2:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == 0);
        assert(sjp->str_bytes == 32);
        assert(memcmp(sjp->str, "ectetur adipiscing elit, sed do ", sjp->str_bytes) == 0);
        break;

    case 3:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == 0);
        assert(sjp->str_bytes == 32);
        assert(memcmp(sjp->str, "eiusmod tempor incididunt ut lab", sjp->str_bytes) == 0);
        break;

    case 4:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == 0);
        assert(sjp->str_bytes == 32);
        assert(memcmp(sjp->str, "ore et dolore magna aliqua. Ut e", sjp->str_bytes) == 0);
        break;

    case 5:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == 0);
        assert(sjp->str_bytes == 32);
        assert(memcmp(sjp->str, "nim ad minim veniam, quis nostru", sjp->str_bytes) == 0);
        break;

    case 6:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == 0);
        assert(sjp->str_bytes == 32);
        assert(memcmp(sjp->str, "d exercitation ullamco laboris n", sjp->str_bytes) == 0);
        break;

    case 7:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == 0);
        assert(sjp->str_bytes == 32);
        assert(memcmp(sjp->str, "isi ut aliquip ex ea commodo con", sjp->str_bytes) == 0);
        break;

    case 8:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == 0);
        assert(sjp->str_bytes == 32);
        assert(memcmp(sjp->str, "sequat. Duis aute irure dolor in", sjp->str_bytes) == 0);
        break;

    case 9:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == 0);
        assert(sjp->str_bytes == 32);
        assert(memcmp(sjp->str, " reprehenderit in voluptate veli", sjp->str_bytes) == 0);
        break;

    case 10:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == 0);
        assert(sjp->str_bytes == 32);
        assert(memcmp(sjp->str, "t esse cillum dolore eu fugiat n", sjp->str_bytes) == 0);
        break;

    case 11:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == 0);
        assert(sjp->str_bytes == 32);
        assert(memcmp(sjp->str, "ulla pariatur. Excepteur sint oc", sjp->str_bytes) == 0);
        break;

    case 12:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == 0);
        assert(sjp->str_bytes == 32);
        assert(memcmp(sjp->str, "caecat cupidatat non proident, s", sjp->str_bytes) == 0);
        break;

    case 13:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == 0);
        assert(sjp->str_bytes == 32);
        assert(memcmp(sjp->str, "unt in culpa qui officia deserun", sjp->str_bytes) == 0);
        break;

    case 14:
        assert(sjp->d == 1);
        assert(e->i == 0);
        assert((e->t & SJP_TYPE) == SJP_STR_T);
        assert((e->t & SJP_FLAGS) == SJP_END);
        assert(sjp->str_bytes == 29);
        assert(memcmp(sjp->str, "t mollit anim id est laborum.", sjp->str_bytes) == 0);
        break;

    case 15:
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
