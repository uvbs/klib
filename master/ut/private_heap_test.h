#ifndef _klib_private_heap_test_h
#define _klib_private_heap_test_h


#include <klib.h>

using namespace klib::mem;
using namespace klib::core;
using namespace klib;

class cheap_test
{

};

TEST(private_heap_test, 1)
{
    private_heap my_heap;

    my_heap.alloc(10);

    private_heap_buffer<cheap_test> buff(my_heap, 32);
        
}


TEST(util,1 )
{
    auto strExt = klib::util::extractFileExt(std::string("ssss.txt"));
    EXPECT_TRUE(strExt == "txt");

    //strExt = klib::core::get_file_ext("ssss.txt");

    auto strExtW = klib::util::extractFileExt(std::wstring(L"hello.txt"));
    EXPECT_EQ(strExtW, L"txt");


    extractFileExt(std::string("dongkun.exe"));
}

#endif
