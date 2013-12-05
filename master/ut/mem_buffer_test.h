#pragma once

#include <core/mem_buffer.h>


TEST(mem_buffer_test, 1)
{
    klib::mem::mem_buffer<555, 55, FALSE> sss;

    sss.append("sdf");
}