// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main.c"


static const char *document = " { \" a \" : 1 , \" b \" : 2 } , [ 3 , 4 ] ";

static void cb(sjp_t *sjp)
{
    DEBUG_SWITCH {
    case 0:
        ASSERT_OBJ_START(0, 0);
        break;

    case 1:
        ASSERT_STR_KEY(1, 0, " a ");
        break;

    case 2:
        ASSERT_NUM(1, 0, 1);
        break;

    case 3:
        ASSERT_STR_KEY(1, 1, " b ");
        break;

    case 4:
        ASSERT_NUM(1, 1, 2);
        break;

    case 5:
        ASSERT_OBJ_END(0, 0);
        break;

    case 6:
        ASSERT_ARRAY_START(0, 1);
        break;

    case 7:
        ASSERT_NUM(1, 0, 3);
        break;

    case 8:
        ASSERT_NUM(1, 1, 4);
        break;

    case 9:
        ASSERT_ARRAY_END(0, 1);
        break;

    default:
        assert(0);
    }
}
