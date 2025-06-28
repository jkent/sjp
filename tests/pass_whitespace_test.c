// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main.c"


static const char *document = " \t\r\n";
static const string_t strings[] = {
    {NULL}
};

static void cb(sjp_t *sjp)
{
    static unsigned int n = 0;

    DEBUG_CB();

    assert(0);
}
