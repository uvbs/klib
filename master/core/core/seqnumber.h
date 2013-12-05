
#pragma once


#include "../kthread/auto_lock.h"

namespace klib {
namespace core {

using namespace  klib::kthread;

class seq_number
{
public:
    seq_number(int64_t seq_id = 0)
    {
        seq_ = seq_id;  
    }

    int64_t alloc_id() 
    {
        auto_lock lock(mutex_);
        return ++ seq_;
    }

protected:
    auto_cs mutex_;
    int64_t seq_;
};

}}