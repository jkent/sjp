// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main.c"


static const char *document = "[3.14159, 42, -0.123456789e+10]";
static const string_t strings[] = {
    {NULL}
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
        ASSERT_NUM(1, 0, 3.14159);
        break;

    case 2:
        ASSERT_NUM(1, 1, 42);
        break;

    case 3:
        ASSERT_NUM(1, 2, -0.123456789e+10);
        break;

    case 4:
        ASSERT_ARRAY_END(0, 0);
        break;

    default:
        assert(0);
        break;
    }
}
