#pragma once

namespace klib{
namespace kthread{

/*
usage: 
typedef ThreadSafeWrapper<std::list<int>> safe_list;

safe_list lst;

if (true) 
{
    lst->push_back(1);
    EXPECT_EQ(lst->size(), 1);

    lst->clear();
    EXPECT_EQ(lst->size(), 0);

    for (int i=0; i<10; ++ i)
    {
        lst->push_back(i);
    }
}

if (true) 
{
    auto& mutex_ = lst._mutex();
    MutexGuard guard(mutex_);

    for (auto itr=lst._wrapped()->begin(); 
        itr != lst._wrapped()->end(); 
        ++ itr)
    {
        printf("%d", *itr);
    }
    printf("\n");
}
*/
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
