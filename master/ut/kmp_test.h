
#pragma once


#include <algo/kmp.h>

TEST(kmp, 1)
{
    klib::algo::kmp<std::string, 100> x;

    auto pos = x.search_once("sabcdefg", "s");
    EXPECT_TRUE(pos == 0);

    const char* pstr = "sdfdsfsdf1234567890dsflkjds";
    x.set_str("1234567890", 10);
    pos = x.search_in(pstr, strlen(pstr));
    EXPECT_TRUE(pos == 9);

}