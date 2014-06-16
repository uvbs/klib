#pragma once

namespace klib{
namespace kthread{


template <typename WrappedType>
class ThreadSafeWrapper : public InterceptAspect<WrappedType, ThreadSafeWrapper<WrappedType>>
{
public:
    ThreadSafeWrapper(WrappedType* p): BaseAspect(p) {}
    ThreadSafeWrapper() : InterceptAspect(&val_) {}

    void _before(WrappedType* p)  {  mutex_.lock();  }
    void _after(WrappedType* p)   {  mutex_.unlock(); }

    WrappedType* _wrapped()   { return wrapped_ptr_; }
    Mutex& _mutex()            { return mutex_;        }

    //重载指针运算符用来织入切面（Before和After）
    std::shared_ptr<WrappedType> operator->() 
    {
        _derived()->_before(wrapped_ptr_);
        return std::shared_ptr<WrappedType>(_wrapped(), 
            [&](WrappedType* p) { this->_after(p); });
    }

protected:
    Mutex mutex_;
    WrappedType val_;
};



}}
