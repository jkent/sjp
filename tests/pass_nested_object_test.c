// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main.c"


static const char *document = "{\"a\":{\"a.a\":{\"a.a.a\":null},\"a.b\":false},\"b\":true}";
static const string_t strings[] = {
    {"a", 1},
    {"a.a", 3},
    {"a.a.a", 5},
    {"a.b", 3},
    {"b", 1},
    {NULL}
};

static void cb(sjp_t *sjp)
{
    static unsigned int n = 0;
    static unsigned int str_idx = 0;
    static unsigned int str_pos = 0;

    DEBUG_CB();

    switch (n++) {
    case 0:
        ASSERT_OBJ_START(0, 0);
        break;

    case 1:
        ASSERT_STR_KEY(1, 0);
        break;

    case 2:
        ASSERT_OBJ_START(1, 0);
        break;

    case 3:
        ASSERT_STR_KEY(2, 0);
        break;

    case 4:
        ASSERT_OBJ_START(2, 0);
        break;

    case 5:
        ASSERT_STR_KEY(3, 0);
        break;

    case 6:
        ASSERT_NULL(3, 0);
        break;

    case 7:
        ASSERT_OBJ_END(2, 0);
        break;

    case 8:
        ASSERT_STR_KEY(2, 1);
        break;

    case 9:
        ASSERT_FALSE(2, 1);
        break;

    case 10:
        ASSERT_OBJ_END(1, 0);
        break;

    case 11:
        ASSERT_STR_KEY(1, 1);
        break;

    case 12:
        ASSERT_TRUE(1, 1);
        break;

    case 13:
        ASSERT_OBJ_END(0, 0);
        break;

    default:
        assert(0);
        break;
    }
}
