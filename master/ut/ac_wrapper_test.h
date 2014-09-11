
#ifndef _klib_ac_wrapper_test_h
#define _klib_ac_wrapper_test_h


#include <algo/search/ac_wrapper.h>
#include <util/rand_str.h>

using namespace klib::algo;
using namespace klib::util;


TEST(ac_wrappertest, 1)
{
    rand_str rstr;

    ac_wrapper ac;

    for (int i = 0; i < 10; i++)
    {
        std::string str = rstr.get_rand_str(i+5);
        ac.add_pattern(str.c_str(), str.size(), (void*)i);
    }

    ac.add_pattern("sdfsf", 5, (void*)333333);
    ac.compile();

    ac.search("cfffffsdfsf", 12);
}






#endif // _klib_ac_wrapper_test_h