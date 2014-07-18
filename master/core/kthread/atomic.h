
#ifndef _klib_atomic_h_
#define _klib_atomic_h_

#include "../inttype.h"

#ifdef WIN32
#include <Winbase.h>
#else
#endif

namespace klib{
namespace kthread{


#ifdef WIN32
    inline LONG atomic_inc(volatile LONG *value)
    { 
        return InterlockedIncrement(value); 
    }
#else
    inline int32_t atomic_inc(volatile int32_t *value)
    {
        __sync_fetch_and_add(value, 1);
        return *value;
    }
#endif

#ifdef WIN32
    inline LONGLONG atomic_inc(volatile LONGLONG *value)
    { 
        return InterlockedIncrement64(value); 
    }
#else
    inline int64_t atomic_inc(volatile int64_t *value)
    {
        __sync_fetch_and_add(value, 1);
        return *value;
    }
#endif

#ifdef WIN32
    inline LONG atomic_dec(volatile LONG *value)
    { 
        return InterlockedDecrement(value); 
    }
#else
    inline int32_t atomic_dec(volatile int32_t *value)
    {
        __sync_fetch_and_sub(value, 1);
        return *value;
    }
#endif

#ifdef WIN32
    inline LONGLONG atomic_dec(volatile LONGLONG *value)
    { 
        return InterlockedDecrement64(value); 
    }
#else
    inline int64_t atomic_dec(volatile int64_t *value)
    {
        __sync_fetch_and_sub(value, 1);
        return *value;
    }
#endif


    template<typename T>
    class atomic_integer
    {
    public:
        atomic_integer()
            : val_(0)
        {}

        T inc() { return atomic_inc(&val_); }
        T dec() { return atomic_dec(&val_); }
        T value() const { return val_; }
    private:
        T val_;
    };

#ifdef WIN32
    typedef atomic_integer<LONG> AtomicInt32;
    typedef atomic_integer<LONGLONG> AtomicInt64;
#else
    typedef atomic_integer<int32_t> AtomicInt32;
    typedef atomic_integer<int64_t> AtomicInt64;
#endif

}}

#endif  // _klib_atomic_h_
