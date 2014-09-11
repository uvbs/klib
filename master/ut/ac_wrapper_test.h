
#ifndef _klib_ac_wrapper_test_h
#define _klib_ac_wrapper_test_h


#include <algo/search/ac_wrapper.h>

using namespace klib::algo;


TEST(ac_wrappertest, 1)
{
    ac_wrapper ac;
    ac.add_pattern("sdfsf", 5, nullptr);
    ac.compile();

    ac.search("cfffffsdfsf", 12);
}






#endif // _klib_ac_wrapper_test_h