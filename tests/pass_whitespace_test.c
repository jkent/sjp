// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main.c"


static const char *document = " \t\r\n";

static void cb(sjp_t *sjp)
{
    DEBUG_SWITCH {
    default:
        assert(0);
    }
}
