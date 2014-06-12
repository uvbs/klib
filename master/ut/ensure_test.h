
#pragma once


#include <rdebug/ensure_nuos.h>

TEST(ensure, 1)
{
    int exp = 1;

    ENSURE(exp == 1)(exp);

}