#pragma once

namespace klib{
namespace kthread{


template <typename WrappedType>
class ThreadSafeWrapper : public InterceptAspect<WrappedType, ThreadSafeWrapper<WrappedType>>
{
public:
    ThreadSafeWrapper(WrappedType* p): BaseAspect(p) {}
    ThreadSafeWrapper() :BaseAspect(&val_) {}

    void Before(WrappedType* p)  {   mutex_.lock();  }
    void After(WrappedType* p)   {  mutex_.unlock(); }

    WrappedType& GetDerived()    { return *m_wrappedPtr; }
    Mutex& GetMutex()            { return mutex_;        }

protected:
    Mutex mutex_;
    WrappedType val_;
};



}}
