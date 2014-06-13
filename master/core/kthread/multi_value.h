#pragma once

#include "../pattern/aspect.h"
#include "auto_lock.h"

namespace klib {
namespace kthread{




template<typename _type>
class multi_safe_val
{
public:
    multi_safe_val() : val_(0) {}
    multi_safe_val(int v) : val_(v) {}

    typedef multi_safe_val self_type;

    operator _type ()
    {
        MutexGuard guard(mutex_);
        return val_;
    }

    self_type& operator ++ ()
    {
        MutexGuard guard(mutex_);
        ++ this->val_;
        return *this;
    }

    self_type& operator + (const int& v)
    {
        MutexGuard guard(mutex_);
        this->val_ += v;
        return *this;
    }

    self_type& operator += (const int& v)
    {
        MutexGuard guard(mutex_);
        this->val_ += v;
        return *this;
    }

    self_type& operator + (const self_type& other)
    {
        MutexGuard guard(mutex_);
        this->val_ += other.val_;
        return *this;
    }

    self_type& operator - (const int& v)
    {
        MutexGuard guard(mutex_);
        this->val_ -= v;
        return *this;
    }

    self_type& operator - (const self_type& other)
    {
        MutexGuard guard(mutex_);
        this->val_ -= other.val_;
        return *this;
    }

    self_type& operator -= (const int& v)
    {
        MutexGuard guard(mutex_);
        this->val_ -= v;
        return *this;
    }

    self_type& operator -= (const self_type& other)
    {
        MutexGuard guard(mutex_);
        this->val_ -= other.val_;
        return *this;
    }

    self_type& operator = (const int& v)
    {
        MutexGuard guard(mutex_);
        this->val_ = v;
        return *this;
    }

    self_type& operator = (const self_type& other)
    {
        MutexGuard guard(mutex_);
        this->val_ = other.val_;
        return *this;
    }

protected:
    _type val_;
    Mutex mutex_;
};

typedef multi_safe_val<int16_t> safe_int16;
typedef multi_safe_val<int32_t> safe_int32;
typedef multi_safe_val<int64_t> safe_int64;

typedef multi_safe_val<uint16_t> safe_uint16;
typedef multi_safe_val<uint32_t> safe_uint32;
typedef multi_safe_val<uint64_t> safe_uint64;


}}