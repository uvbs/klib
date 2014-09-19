#include "rand_val.h"

using namespace klib::util;

rand_val::rand_val()
    : min_val_(0)
    , max_val_(0)
{}

rand_val::rand_val(size_t minv, size_t maxv)
{
    min_val_ = minv;
    max_val_ = maxv;
}

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

rand_val::operator size_t()
{
    if (min_val_ == 0 && max_val_ == 0)
    {
        return val();
    }

    return val(min_val_, max_val_);
}