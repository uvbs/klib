#pragma once

#include <klib.h>
#include <core/singleton.h>
#include <tuple>


class nocopyable_test : public klib::noncopyable
{

};

class MySingleton : public singleton<MySingleton>
{
    DECLARE_SINGLETON_CLASS(MySingleton);

};


TEST(nocopyable_test, 1)
{
    nocopyable_test obj1;
    nocopyable_test obj2;


    // compile error 
    //obj1 = obj2;

    MySingleton::instance();
    //MySingleton a;    // not allow
    //MySingleton b;    // not allow
    // a = b; 

    std::tuple<float,std::string> mytuple (3.14f, "pi");

    std::pair<int,char> mypair (10,'a');

    auto myauto = mypair;
    std::cout << "myauto contains: " << std::endl;
    std::cout << std::get<0>(myauto) << std::endl;
    std::cout << std::get<1>(myauto) << std::endl;
}