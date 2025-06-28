// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main.c"


static const char *document = "[[],[[]]]";
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
        ASSERT_ARRAY_START(1, 0);
        break;

    case 2:
        ASSERT_ARRAY_END(1, 0);
        break;

    case 3:
        ASSERT_ARRAY_START(1, 1);
        break;

    case 4:
        ASSERT_ARRAY_START(2, 0);
        break;

    case 5:
        ASSERT_ARRAY_END(2, 0);
        break;

    case 6:
        ASSERT_ARRAY_END(1, 1);
        break;

    case 7:
        ASSERT_ARRAY_END(0, 0);
        break;

    default:
        assert(0);
        break;
    }
}
