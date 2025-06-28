// SPDX-License-Identifer: MIT

#include <assert.h>

#include "main.c"


static const char *document = "{\"a\" null}";
static const string_t strings[] = {
    {"a", 1},
    {NULL},
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
        ASSERT_NULL(1, 0);
        break;

    case 3:
        ASSERT_ARRAY_END(0, 0);
        break;

    default:
        assert(0);
        break;
    }
}
