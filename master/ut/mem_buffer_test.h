#ifndef _klib_mem_buffer_test_h
#define _klib_mem_buffer_test_h

#include <mem/mem_buffer.h>


TEST(mem_buffer_test, 1)
{
    klib::mem::mem_buffer<555, 55, FALSE> sss;

    sss.append("sdf");
}

#endif
