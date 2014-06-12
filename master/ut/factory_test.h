
#ifndef _klib_factory_test_h
#define _klib_factory_test_h

#include <pattern/factory.h>



int* test()
{
    return new int;
}


TEST(factory_, 1)
{

    klib::pattern::factory<int>* fa = klib::pattern::factory<int>::instance();
    fa->add(_T("sdf"), test);
    int * p = fa->make(_T("sdf"));



}

#endif
