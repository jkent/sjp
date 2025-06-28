// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main.c"


static const char *document = "[]";
static const string_t strings[] = {
    {NULL},
};

static void cb(sjp_t *sjp)
{
    static unsigned int n = 0;

    DEBUG_CB();

    switch (n++) {
    case 0:
        ASSERT_ARRAY_START(0, 0);
        break;

    case 1:
        ASSERT_ARRAY_END(0, 0);
        break;

    default:
        assert(0);
        break;
    }
}
