
#ifndef _klib_mem_check_h
#define _klib_mem_check_h

#include <map>
#include <set>
#include <string>
#include <windows.h>

#include "../include/allocator.h"
#include "../include/lock.h"
#include "addr_mgr.h"



/**
 1、支持容器的申请
 2、支持内存块申请(需要添加使用说明)
 3、输出统计信息 
*/

namespace klib {
namespace debug {



// 类的内存管理类
class class_mem_mgr
{
public:
    struct class_mem_info
    {
        class_mem_info() : ncurr_size(0), nmax_size(0), nitem_size(0), nitem_count(0), nalloc_count(0), nfree_count(0)
           { class_name[0] = 0; }

        bool operator < (const class_mem_info& other)
        {
            if (strcmp(this->class_name, other.class_name) < 0) 
            {
                return true;
            }

            return false;
        }

        char class_name[50];
        size_t ncurr_size;
        size_t nmax_size;

        size_t nitem_size;
        size_t nitem_count;

        size_t nalloc_count;
        size_t nfree_count;
    };


    ~class_mem_mgr();

    static class_mem_mgr* instance();
    auto_cs& get_lock();

    void add_class_mem(const char* class_name, void* p, size_t nsize);
    void del_class_mem(const char* class_name, void* p);
    std::string summary();

protected:
    std::map<const char*,
            class_mem_info,
            std::less<const char*>, 
            MemAlloc<std::pair<const char*, class_mem_info> >> m_class_infos;

    addr_mgr addr_mgr_;
    auto_cs  m_cs;
};

// 全局内存统计
class global_mem_mgr
{
public:
    // global memory info
    struct global_mem_info
    {
        global_mem_info() 
            : ncurr_size(0)
            , nmax_size(0)
            , nalloc_count(0)
            , nfree_count(0) 
        {}

        size_t ncurr_size;           // 当前大小
        size_t nmax_size;            // 最大大小

        size_t nalloc_count;          // 申请的次数
        size_t nfree_count;          // 释放的次数
    };

    static global_mem_mgr* instance();
    auto_cs& get_lock();

    void add_global_mem(void* p, size_t nsize);
    void del_global_mem(void* p);

    std::string summary() const;
    std::string summary_xml(const char* module_name) const; // 格式化成xml格式
    
protected:
    addr_mgr addr_mgr_;
    mutable auto_cs  m_cs;
    global_mem_info mem_info_;
};

class mem_imp
{
public:
    static void* alloc_class(const char* class_name, size_t nsize); // 类名与大小
    static void* alloc_global(size_t nsize); // 全局的

    static void free_class(const char* class_name, void* p);
    static void free_global(void* p);

    static void enable_stats(bool benable);
    static std::string summary();   // 汇总，包括总内存，及各类的使用内存
};


}}


// 控制是否启用内存统计
#define  ENABLE_MEMORY_STATS   1

#ifdef ENABLE_MEMORY_STATS

// 统计类的内存使用，在class里面申明
#define  STATS_CLASS_MEMORY(theClass)                                    \
                                                                         \
    void* operator new(size_t obj_size)                                  \
    {                                                                    \
        return mem::mem_imp::alloc_class(#theClass, obj_size);           \
    }                                                                    \
                                                                         \
    void* operator new[](size_t arr_size)                                \
    {                                                                    \
        return mem::mem_imp::alloc_class(#theClass, arr_size);           \
    }                                                                    \
                                                                         \
    void operator delete(void* obj_addr)                                 \
    {                                                                    \
        mem::mem_imp::free_class(#theClass, obj_addr);                   \
    }                                                                    \
                                                                         \
    void operator delete[](void* arr_addr)                               \
    {                                                                    \
        mem::mem_imp::free_class(#theClass, arr_addr);                   \
    }



// 统计全局的内存使用，在一个地方定义即可
#define  STATS_GLOBAL_MEMORY()                                               \
    void* operator new(size_t type_size)                                     \
    {                                                                        \
        return ::mem::mem_imp::alloc_global(type_size);                      \
    }                                                                        \
                                                                             \
    void* operator new[](size_t type_size)                                   \
    {                                                                        \
        return ::mem::mem_imp::alloc_global(type_size);                      \
    }                                                                        \
                                                                             \
    void * operator new(size_t type_size, const _STD nothrow_t&)             \
    {                                                                        \
        return ::mem::mem_imp::alloc_global(type_size);                      \
    }                                                                        \
                                                                             \
    void * operator new[](size_t type_size, const _STD nothrow_t&)           \
    {                                                                        \
        return ::mem::mem_imp::alloc_global(type_size);                      \
    }                                                                        \
                                                                             \
    void operator delete(void* obj_ptr)                                      \
    {                                                                        \
        ::mem::mem_imp::free_global(obj_ptr);                                \
    }                                                                        \
                                                                             \
    void operator delete[](void* obj_ptr)                                    \
    {                                                                        \
        ::mem::mem_imp::free_global(obj_ptr);                                \
    }                                                                        \
                                                                             \
    void  operator delete(void * obj_ptr, const _STD nothrow_t&)             \
    {                                                                        \
        ::mem::mem_imp::free_global(obj_ptr);                                \
    }                                                                        \
                                                                             \
    void  operator delete[](void *obj_ptr, const _STD nothrow_t&)            \
    {                                                                        \
        ::mem::mem_imp::free_global(obj_ptr);                                \
    }                                                                         

#else

#define STATS_CLASS_MEMORY(theClass)
#define STATS_GLOBAL_MEMORY()

#endif




#endif
