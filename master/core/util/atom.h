
#pragma once

#include "../core/noncopyable.h"

#include "../kthread/auto_lock.h"

using namespace klib::core;
using namespace klib::kthread;

namespace klib {
namespace util{


class AtomicInt : noncopyable
{
public:
    AtomicInt() : value_(0) {}

    LONG get() { return InterlockedCompareExchange(&value_, 0, 0); }
    LONG set(LONG newValue) { return InterlockedExchange(&value_, newValue); }
    LONG getAndAdd(LONG x) { return InterlockedExchangeAdd(&value_, x); }
    LONG addAndGet(LONG x) { return getAndAdd(x) + x; }
    LONG increment() { return InterlockedIncrement(&value_); }
    LONG decrement() { return InterlockedDecrement(&value_); }
    LONG getAndSet(LONG newValue) { return set(newValue); }

private:
    volatile LONG value_;
};

class AtomicInt64 : noncopyable
{
public:
    AtomicInt64() : value_(0) {}

    INT64 get()
    {
        auto_lock locker(mutex_);
        return value_;
    }
    INT64 set(INT64 newValue)
    {
        auto_lock locker(mutex_);
        INT64 temp = value_;
        value_ = newValue;
        return temp;
    }
    INT64 getAndAdd(INT64 x)
    {
        auto_lock locker(mutex_);
        INT64 temp = value_;
        value_ += x;
        return temp;
    }
    INT64 addAndGet(INT64 x)
    {
        auto_lock locker(mutex_);
        value_ += x;
        return value_;
    }
    INT64 increment()
    {
        auto_lock locker(mutex_);
        ++value_;
        return value_;
    }
    INT64 decrement()
    {
        auto_lock locker(mutex_);
        --value_;
        return value_;
    }
    INT64 getAndSet(INT64 newValue)
    {
        return set(newValue);
    }

private:
    volatile INT64 value_;
    klib::kthread::auto_cs  mutex_;
};



}}