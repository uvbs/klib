#ifndef _klib_object_creator_h_
#define _klib_object_creator_h_


#include "mem_lib.h"


template<class T>
class object_creator
{
public:
    typedef object_creator<T>       self_type;
    typedef unsigned long long *    pointer_type;

    object_creator() : m_desc(nullptr) {}
    object_creator(const char* desc)
        : m_desc(desc)
    {}

#define  check_get_ptr(Type)  \
    T* ptr = (T*)mem_lib::instance()->alloc_global(sizeof(T), m_desc); \
    if (nullptr == ptr)         \
    {                           \
        return ptr;             \
    }

public:
    T * create()
    {
        check_get_ptr(T);

        new (ptr) T;
        return ptr;
    }

    template <class tr1>
    T * create(tr1 t1)
    {
        check_get_ptr(T);

        new (ptr) T(t1);
        return ptr;
    }

    template <class tr1, class tr2>
    T * create(tr1 t1, tr2 t2)
    {
        check_get_ptr(T);

        new (ptr) T(t1, t2);
        return ptr;
    }

    template <class tr1, class tr2, class tr3>
    T * create(tr1 t1, tr2 t2, tr3 t3)
    {
        check_get_ptr(T);

        new (ptr) T(t1, t2, t3);
        return ptr;
    }

    template <class tr1, class tr2, class tr3, class tr4>
    T * create(tr1 t1, tr2 t2, tr3 t3, tr4 t4)
    {
        check_get_ptr(T);

        new (ptr) T(t1, t2, t3, t4);
        return ptr;
    }

    template <class tr1, class tr2, class tr3, class tr4, class tr5>
    T * create(tr1 t1, tr2 t2, tr3 t3, tr4 t4, tr5 t5)
    {
        check_get_ptr(T);

        new (ptr) T(t1, t2, t3, t4, t5);
        return ptr;
    }

    template <class tr1, class tr2, class tr3, class tr4, class tr5, class tr6>
    T * create(tr1 t1, tr2 t2, tr3 t3, tr4 t4, tr5 t5, tr6 t6)
    {
        check_get_ptr(T);

        new (ptr) T(t1, t2, t3, t4, t5, t6);
        return ptr;
    }

protected:
    const char*  m_desc;
};


//@todo 需要记录行数，类型名，及大小
#define Type(t)  object_creator<t>()



#endif // _klib_object_creator_h_