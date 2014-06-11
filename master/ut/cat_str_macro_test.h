
#ifndef _klib_cat_str_macro_test_h
#define _klib_cat_str_macro_test_h


TEST(util, cat_str_test)
{
    EXPECT_EQ(CAT_3_STR(first, _, second), std::string("first_second"));

    EXPECT_EQ(CAT_2_STR(first, second), std::string("firstsecond"));
}


#endif
