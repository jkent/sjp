// SPDX-License-Identifer: MIT

#include <assert.h>

#define CHUNKSIZE 1
#include "main.c"


static const char *document = "{\"array\":[1,true,\"string\"],\"object\":{\"key\":\"value\"},\"sentence\":\"The quick brown fox jumped over the lazy dog.\",\"emoji\":\"👍\"}";

static void cb(sjp_t *sjp)
{
    DEBUG_SWITCH {
    case 0:
        ASSERT_OBJ_START(0, 0);
        break;

    case 1:
        ASSERT_STR_KEY(1, 0, "array");
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
        ASSERT_STR_VALUE(2, 2, "string");
        break;

    case 6:
        ASSERT_ARRAY_END(1, 0);
        break;

    case 7:
        ASSERT_STR_KEY(1, 1, "object");
        break;

    case 8:
        ASSERT_OBJ_START(1, 1);
        break;

    case 9:
        ASSERT_STR_KEY(2, 0, "key");
        break;

    case 10:
        ASSERT_STR_VALUE(2, 0, "value");
        break;

    case 11:
        ASSERT_OBJ_END(1, 1);
        break;

    case 12:
        ASSERT_STR_KEY(1, 2, "sentence");
        break;

    case 13:
        ASSERT_STR_VALUE(1, 2, "The quick brown fox jumped over the lazy dog.");
        break;

    case 14:
        ASSERT_STR_KEY(1, 3, "emoji");
        break;

    case 15:
        ASSERT_STR_VALUE(1, 3, "👍");
        break;

    case 16:
        ASSERT_OBJ_END(0, 0);
        break;

    default:
        assert(0);
    }
}
