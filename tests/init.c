// SPDX-License-Identifer: MIT

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sjp.h"


static void sjp_cb(sjp_t *sjp);

int main(int argc, char *argv[])
{
    sjp_t *sjp;

    sjp = sjp_init(sjp_cb);
    assert(sjp != NULL);

    sjp_free(sjp);

    exit(EXIT_SUCCESS);
    return 0;
}

static void sjp_cb(sjp_t *sjp)
{
}
