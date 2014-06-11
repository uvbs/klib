#ifndef _klib_must_have_base_h
#define _klib_must_have_base_h

/*
前置条件是指函数履行其契约所必须满足的条件。满足前置条件是调用者的责任。
倘若调用者没有满足前置条件，则被调用者做出任何事情都是完全合理的。

后置条件在函数执行完毕时必须为真。确保后置条件被满足是被调用者的责任。
前置条件和后置条件可以被应用到类的成员函数，也可以被用到自由函数身上。

动态断言及静态断言的使用
*/
template<typename D, typename B>
struct must_have_base
{
    ~must_have_base(void)
    {
        void (*p)(D *, B *) = constraints;
    }
private:
    static void constraints(IN D *pd, IN B *pb)
    {
        pb = pd;
    }
};




#endif