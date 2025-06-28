// SPDX-License-Identifer: MIT

#include <stdlib.h>

#include "sjp.h"
#include "common.h"

#ifndef CHUNKSIZE
# define CHUNKSIZE 16
#endif

int main(int argc, char *argv[])
{
    const char *p;
    unsigned int len, chunk;
    int ret;
    sjp_t *sjp;

    (void) strings;

    sjp = sjp_init(cb);
    assert(sjp != NULL);

    p = document;
    len = strlen(p);
#ifndef CHUNKSIZE
    chunk = len;
#else
    chunk = CHUNKSIZE;
#endif
    chunk = (len < chunk) ? len : chunk;
    while (len > 0 && (ret = sjp_parse(sjp, p, chunk)) >= 0) {
        p += chunk;
        len -= chunk;
        chunk = (len < chunk) ? len : chunk;
    }

    assert(ret >= 0);

    sjp_free(sjp);

    exit(EXIT_SUCCESS);
    return 0;
}
