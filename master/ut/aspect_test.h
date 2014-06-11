#ifndef _klib_aspect_test_h
#define _klib_aspect_test_h

#include <iostream>
using namespace klib::pattern;

using namespace std;

/// 包装的类
template <typename WrappedType>
class LoggingAspect : public InterceptAspect<WrappedType, LoggingAspect<WrappedType> >
{
    typedef  InterceptAspect<WrappedType, LoggingAspect<WrappedType> > InterceptAspect;
public:
    LoggingAspect(WrappedType* p): InterceptAspect(p) {}

    void Before(WrappedType* p)
    {
        std::cout << typeid(*p).name() <<  "---->> entering" << std::endl;
    }

    void After(WrappedType* p)
    {
        std::cout << typeid(*p).name() << "---->> exiting" << std::endl << std::endl;
    }

    void g()
    {
        m_wrappedPtr->g();
        cout << "g()" << endl;
    }
};

// 过滤
class IX
{
public:
    IX(){}
    virtual ~IX(){}

    virtual void g()=0;
private:

};

class X : public IX
{
public:
    void g() 
    {
        std::cout << "it is a test" << std::endl;
    }

    void test_echo()
    {
        std::cout << "test_echo :: hello world" << std::endl;
    }

    void test_param(char* pszParam)
    {
        std::cout << "test_param :: " << pszParam << std::endl;
    }
};

TEST(aspect, 1)
{
    X x;

    LoggingAspect<X> object(&x);
    MakeAspect<LoggingAspect>(&object)->g();
}

#endif
