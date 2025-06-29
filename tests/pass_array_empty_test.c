// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main.c"


static const char *document = "[]";

static void cb(sjp_t *sjp)
{
    DEBUG_SWITCH {
    case 0:
        ASSERT_ARRAY_START(0, 0);
        break;

    case 1:
        ASSERT_ARRAY_END(0, 0);
        break;

    default:
        assert(0);
    }
}
