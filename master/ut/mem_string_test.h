#ifndef _klib_mem_string_test_h
#define _klib_mem_string_test_h


//#include "core/mem_string.h"
#include "mem/mem_buffer.h"

TEST(mem_buffer, create)
{
    klib::mem::mem_buffer<56*1024, 26, FALSE> buff;
    buff.append("sdfsdf");
    buff.append("---");
    buff.append("sdfsdf", 6);
    buff.append(buff);

    buff.erase_back(6);
    char szbuff[50];
    buff.copy(szbuff, 0, buff.size());


    klib::mem::mem_file_wrapper<56*1024, 1024, FALSE> buff_file;
    
    buff_file.write("sdfsdfsdf", 8);
    buff_file.append("sdfsf");
    buff_file.clear();
    ASSERT_TRUE(buff_file.size() == 0);
    ASSERT_TRUE(buff_file.empty());

    buff_file << "sdfdsfdsf";

    buff_file.append("sdfsdf");
    buff_file.seekg(0, buff_file.self_type::way_begin);
    buff_file.seekg(0, buff_file.self_type::way_cur);
    buff_file.seekg(0, buff_file.self_type::way_end);
}

#endif
