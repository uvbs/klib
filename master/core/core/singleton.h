#pragma once

#include "klib_core.h"


namespace klib{
namespace core{
    

///< 单例模式
template<class T> 
class singleton : public klib::core::noncopyable
{   
public:
    ///< 获取唯一实例对象
    static T* instance()
    {   
        //T有一个protected默认构造函数
        static T _instance; 
        return & _instance;
    }

};

// 申请一下为单例类
#define DECLARE_SINGLETON_CLASS(T)      \
private:                                \
    friend singleton<T>;               \
    T(){}                               \
    T(T&);                              \
public:


}}

