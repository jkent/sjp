// SPDX-License-Identifer: MIT

#include <stdlib.h>

#include "sjp.h"
#include "common.h"


int main(int argc, char *argv[])
{
    const char *p;
    unsigned int len;
    int ret;
    sjp_t *sjp;

    (void) strings;

    sjp = sjp_init(cb);
    assert(sjp != NULL);

    p = document;
    len = strlen(p);
    while ((ret = sjp_parse(sjp, p, len)) > 0) {
        p += ret;
        len -= ret;
    }

    assert(ret >= 0);

    sjp_free(sjp);

    exit(EXIT_SUCCESS);
    return 0;
}
