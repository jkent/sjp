// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main_bytewise.c"


static const char *document = "{\"array\":[1,true,\"string\"],\"object\":{\"key\":\"value\"},\"sentence\":\"The quick brown fox jumped over the lazy dog.\"}";
static const string_t strings[] = {
    {"array", 5},
    {"string", 6},
    {"object", 6},
    {"key", 3},
    {"value", 5},
    {"sentence", 8},
    {"The quick brown fox jumped over the lazy dog.", 45},
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
        ASSERT_ARRAY_START(1, 0);
        break;

    case 3:
        ASSERT_NUM(2, 0, 1);
        break;

    case 4:
        ASSERT_TRUE(2, 1);
        break;

    case 5:
        ASSERT_STR_VALUE(2, 2);
        break;

    case 6:
        ASSERT_ARRAY_END(1, 0);
        break;

    case 7:
        ASSERT_STR_KEY(1, 1);
        break;

    case 8:
        ASSERT_OBJ_START(1, 1);
        break;

    case 9:
        ASSERT_STR_KEY(2, 0);
        break;

    case 10:
        ASSERT_STR_VALUE(2, 0);
        break;

    case 11:
        ASSERT_OBJ_END(1, 1);
        break;

    case 12:
        ASSERT_STR_KEY(1, 2);
        break;

    case 13:
        ASSERT_STR_VALUE(1, 2);
        break;

    case 14:
        ASSERT_OBJ_END(0, 0);
        break;

    default:
        assert(0);
        break;
    }
}
