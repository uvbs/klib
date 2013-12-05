#pragma once


#include <util/util.h>
#include <random>


TEST(util, ×Ö·û´®²âÊÔ)
{
    __int64 iv = 99999;
    double dv = 19.9;

    std::string str_v = klib::util::toString(iv);
    EXPECT_TRUE(str_v == std::to_string(iv));

    str_v = klib::util::toString(dv);
    EXPECT_TRUE(str_v == "19.9");
    str_v = klib::util::toHexString(iv);
    __int64 i_v = klib::util::parseHexString<__int64>(str_v);
    EXPECT_EQ(i_v, iv);


    std::wstring str_w = klib::util::toWString(dv);

    float f = 1.0;
    std::string str_f = klib::util::to_string(f);
    EXPECT_TRUE(str_f == "1.0");

    /*
    std::random_device rd;
    for(int n=0; n<20000; ++n)
        std::cout << rd() << std::endl;
    */
}