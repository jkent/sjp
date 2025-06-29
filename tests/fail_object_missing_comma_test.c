// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main.c"


static const char *document = "{\"a\":null \"b\":null}";

static void cb(sjp_t *sjp)
{
    DEBUG_SWITCH {
    case 0:
        ASSERT_OBJ_START(0, 0);
        break;

    case 1:
        ASSERT_STR_KEY(1, 0, "a");
        break;

    case 2:
        ASSERT_NULL(1, 0);
        break;

    case 3:
        ASSERT_STR_KEY(1, 1, "b");
        break;

    case 4:
        ASSERT_NULL(1, 1);
        break;

    case 5:
        ASSERT_ARRAY_END(0, 0);
        break;

    default:
        assert(0);
    }
}
