
#ifndef _klib_seqnumber_h
#define _klib_seqnumber_h


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

    operator int64_t()
    {
        return alloc_id();
    }

protected:
    mutex mutex_;
    int64_t seq_;
};

}}

#endif
