// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main.c"


static const char *document = "[true, false, null]";
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
        ASSERT_TRUE(1, 0);
        break;

    case 2:
        ASSERT_FALSE(1, 1);
        break;

    case 3:
        ASSERT_NULL(1, 2);
        break;

    case 4:
        ASSERT_ARRAY_END(0, 0);
        break;

    default:
        assert(0);
        break;
    }
}
