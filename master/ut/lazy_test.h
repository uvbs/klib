#pragma once


#include <pattern/lazy.h>
using namespace klib::pattern;

TEST(lazy, 1)
{
    lazy<int> va([](int& t){ t = 1;} );
    int& vx = *va;
    KLIB_ASSERT(vx == 1);
    
    lazy<int> lzv(LAZY_INIT( t = 10;) );
    int& aaa = *lzv;
    KLIB_ASSERT(aaa == 10);
    

}