#include "rand_val.h"

using namespace klib::util;

size_t rand_val::val()
{
    return rd_();
}

size_t rand_val::val(size_t minv, size_t maxv)
{
    size_t dif = maxv - minv;
    dif += 1;

    size_t index = rd_() % dif;
    return minv + index;
}

