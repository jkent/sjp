// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main.c"


static const char *document = "[\"Lorem ipsum dolor sit amet, consectetur "
        "adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
        "magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation "
        "ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute "
        "irure dolor in reprehenderit in voluptate velit esse cillum dolore eu "
        "fugiat nulla pariatur. Excepteur sint occaecat cupidatat non "
        "proident, sunt in culpa qui officia deserunt mollit anim id est "
        "laborum.\"]";

static void cb(sjp_t *sjp)
{
    DEBUG_SWITCH {
    case 0:
        ASSERT_ARRAY_START(0, 0);
        break;

    case 1:
        ASSERT_STR_VALUE(1, 0, "Lorem ipsum dolor sit amet, consectetur "
        "adipiscing elit, sed do eiusmod tempor incididunt ut labore et "
        "dolore magna aliqua. Ut enim ad minim veniam, quis nostrud "
        "exercitation ullamco laboris nisi ut aliquip ex ea commodo "
        "consequat. Duis aute irure dolor in reprehenderit in voluptate velit "
        "esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat "
        "cupidatat non proident, sunt in culpa qui officia deserunt mollit "
        "anim id est laborum.");
        break;

    case 2:
        ASSERT_ARRAY_END(0, 0);
        break;

    default:
        assert(0);
    }
}
