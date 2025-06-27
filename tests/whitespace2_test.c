// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main.c"


static const char *document = " { \" a \" : 1 , \" b \" : 2 } , [ 3 , 4 ] ";
static const string_t strings[] = {
    {" a ", 3},
    {" b ", 3},
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
        ASSERT_NUM(1, 0, 1);
        break;

    case 3:
        ASSERT_STR_KEY(1, 1);
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
        break;
    }
}
