
#pragma once



TEST(debug, must_have_base_)
{
    class test_base
    {

    };

    class test_child : public test_base
    {

    };
    
    // 能够定义变量的静态断言
    typedef must_have_base<test_child, test_base> static_verify_exp;
    static_verify_exp a;
}