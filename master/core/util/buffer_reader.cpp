//#include "stdafx.h"
#include "buffer_reader.h"
#include <memory.h>
#include <stdlib.h>
#include <locale>

namespace klib {
namespace util {


void buffer_reader::set_buff(const unsigned char* buff, size_t len)
{
    cur_ptr  = buff;
    buff_ptr = buff;
    buff_len = len;
    end_ptr = buff_ptr + len;;
}

void buffer_reader::set_buff(const char* buff, size_t len)
{
    set_buff((ptr_type)buff, len);
}

bool buffer_reader::read(unsigned char* buff, size_t len)
{
    if (cur_ptr + len > end_ptr) 
    {
        return false;
    }

    memcpy(buff, cur_ptr, len);
    cur_ptr += len;
    return true;
}

bool buffer_reader::parser_value(size_t& val,size_t base)
{
    if (size() < 0) 
    {
        return false;
    }

    char* tmp_ptr;
    val = strtoul((char*) get_ptr(), &tmp_ptr, base);
    return true;
}

bool buffer_reader::back(size_t len)
{
    if (cur_ptr - len < buff_ptr) 
    {
        return false;
    }

    cur_ptr -= len;
    return true;
}

bool buffer_reader::skip(size_t len)
{
    if (cur_ptr + len > end_ptr) 
    {
        return false;
    }

    cur_ptr += len;
    return true;
}

bool buffer_reader::skip_until(unsigned char ch)
{
    while (cur_ptr < end_ptr)
    {
        if (*cur_ptr == ch) 
        {
            break;
        }

        ++ cur_ptr;
    }

    return cur_ptr != end_ptr;
}

bool buffer_reader::skip_space()
{
    while (cur_ptr < end_ptr)
    {
        if (!isspace(*cur_ptr)) 
        {
            break;
        }

        ++ cur_ptr;
    }

    return cur_ptr != end_ptr;
}

buffer_reader::self_type& buffer_reader::operator ++ ()
{
    if (cur_ptr > end_ptr) 
    {
        ++ cur_ptr;
        return *this;
    }
    return *this;
}

buffer_reader::self_type& buffer_reader::operator ++ (int)
{
    return ++ *this;
}


}}