
#pragma once


#include <rdebug/ensure.h>

TEST(ensure, 1)
{
    int exp = 1;

    ENSURE_ASSERT(exp == 1)(exp);
    ENSURE_VERIFY(exp == 1)(exp);
}