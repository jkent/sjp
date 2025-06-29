// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main.c"


static const char *document = "[], {} []{}";

static void cb(sjp_t *sjp)
{
    DEBUG_SWITCH {
    case 0:
        ASSERT_ARRAY_START(0, 0);
        break;

    case 1:
        ASSERT_ARRAY_END(0, 0);
        break;

    case 2:
        ASSERT_OBJ_START(0, 1);
        break;

    case 3:
        ASSERT_OBJ_END(0, 1);
        break;

    case 4:
        ASSERT_ARRAY_START(0, 2);
        break;

    case 5:
        ASSERT_ARRAY_END(0, 2);
        break;

    case 6:
        ASSERT_OBJ_START(0, 3);
        break;

    case 7:
        ASSERT_OBJ_END(0, 3);
        break;

    default:
        assert(0);
    }
}
