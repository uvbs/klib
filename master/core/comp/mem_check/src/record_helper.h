#ifndef _klib_record_helper_h_
#define _klib_record_helper_h_




template<class T>
class object_creator
{
public:
    typedef object_creator<T>       self_type;
    typedef unsigned long long *    pointer_type;

public:
    T * create()
    {
        return new T;
    }

    template <class tr1>
    T * create(tr1 t1)
    {
        return nullptr;
    }

    template <class tr1, class tr2>
    T * create(tr1 t1, tr2 t2)
    {
        return nullptr;
    }

    template <class tr1, class tr2, class tr3>
    T * create(tr1 t1, tr2 t2, tr3 t3)
    {
        return nullptr;
    }

    template <class tr1, class tr2, class tr3, class tr4>
    T * create(tr1 t1, tr2 t2, tr3 t3, tr4 t4)
    {
        return nullptr;
    }

    template <class tr1, class tr2, class tr3, class tr4, class tr5>
    T * create(tr1 t1, tr2 t2, tr3 t3, tr4 t4, tr5 t5)
    {
        return nullptr;
    }
};


//@todo 需要记录行数，类型名，及大小
#define Type(t)  object_creator<t>()



#endif // _klib_record_helper_h_