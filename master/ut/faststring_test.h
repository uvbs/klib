#pragma once


#include <core/fast_string.h>
using namespace klib::core;



TEST(FastString, combine)
{
    const char * p = "This is a Test";
    std::string s = MakeFastString("Hello") + " " + string("World") + "!" + p + p;
    //cout << s;

    std::string str = MakeFastString("test-string") + "-another";
    ASSERT_TRUE(str == "test-string-another");
}
