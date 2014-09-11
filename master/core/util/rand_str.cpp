
#include "rand_str.h"

#include <random>

#ifndef _WIN32
#include <string.h>
#endif


const char g_rand_str_tbl[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

using namespace klib::util;

std::random_device g_rd;

rand_str::rand_str() 
    : str_tbl_(nullptr)
    , tbl_len_(0)
{
    str_tbl_ = g_rand_str_tbl;
    tbl_len_ = strlen(g_rand_str_tbl);
}

bool rand_str::set_str_tbl(const char* buf, size_t len)
{
    str_tbl_ = buf;
    tbl_len_ = len;
    return len > 0;
}

std::string rand_str::get_rand_str(size_t len)
{
    std::string str_tmp;
    str_tmp.reserve(len);

    for (size_t index = 0; index < len; index++)
    {
        size_t offset = g_rd() % tbl_len_;
        str_tmp.append(str_tbl_ + offset, 1);
    }
    return std::move(str_tmp);
}