#ifndef _klib_aspec_h
#define _klib_aspec_h


#include <memory>
#include <iostream>
using namespace std;


namespace klib{
namespace pattern{


//----------------------------------------------------------------------
// 面向方面编程示例

template <typename WrappedType, typename DerivedAspect>
class BaseAspect
{
protected:
    WrappedType* m_wrappedPtr; //被织入的对象

    //获取派生的切面对象
    DerivedAspect* GetDerived() 
    {
        return static_cast<DerivedAspect*>(this);
    }

    //被织入对象的删除器，用来自动触发切面中的After方法
    struct AfterWrapper
    {
        DerivedAspect* m_derived;
        AfterWrapper(DerivedAspect* derived): m_derived(derived) {};
        void operator()(WrappedType* p)
        {
            m_derived->After(p);
        }
    };
public:
    explicit BaseAspect(WrappedType* p) :  m_wrappedPtr(p) {};


    void Before(WrappedType* p) {
        // Default does nothing
    };

    void After(WrappedType* p) {
        // Default does nothing
    }

    //重载指针运算符用来织入切面（Before和After）
    std::shared_ptr<WrappedType> operator->() 
    {
        GetDerived()->Before(m_wrappedPtr);
        return std::shared_ptr<WrappedType>(m_wrappedPtr, AfterWrapper(GetDerived()));
    }
};

template <typename WrappedType, typename DerivedAspect>
class InterceptAspect
{
protected:
    WrappedType* m_wrappedPtr; //被织入的对象

    //获取派生的切面对象
    DerivedAspect* GetDerived() 
    {
        return static_cast<DerivedAspect*>(this);
    }

    //被织入对象的删除器，用来自动触发切面中的After方法
    struct AfterWrapper
    {
        WrappedType* m_wrappered;
        AfterWrapper(WrappedType* wrapperd): m_wrappered(wrapperd) {};
        void operator()(DerivedAspect* p)
        {
            p->After(m_wrappered);
        }
    };
public:
    explicit InterceptAspect(WrappedType* p) :  m_wrappedPtr(p) {};


    void Before(WrappedType* p) {
        // Default does nothing
    };

    void After(WrappedType* p) {
        // Default does nothing
    }

    //重载指针运算符用来织入切面（Before和After）
    std::shared_ptr<DerivedAspect> operator->() 
    {
        GetDerived()->Before(m_wrappedPtr);
        return std::shared_ptr<DerivedAspect>(GetDerived(), AfterWrapper(m_wrappedPtr));
    }
};


//织入切面的工厂函数, 返回包含被织入对象的切面
template <template <typename> class Aspect, typename WrappedType> 
Aspect<WrappedType> MakeAspect(WrappedType* p) 
{
    return Aspect<WrappedType>(p);
}



}} // namespace



/**
ex:


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

template <typename WrappedType>
class LoggingAspectRaw : public BaseAspect<WrappedType, LoggingAspectRaw<WrappedType> >
{
typedef  BaseAspect<WrappedType, LoggingAspectRaw<WrappedType> > BaseAspect;
public:
LoggingAspectRaw(WrappedType* p): BaseAspect(p) {}

void Before(WrappedType* p) 
{
std::cout << typeid(*p).name() <<  "---->> entering" << std::endl;
}

void After(WrappedType* p) 
{
std::cout << typeid(*p).name() << "---->> exiting" << std::endl << std::endl;
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

// 测试
void TestAop()
{
std::shared_ptr<IX> p(new X());

//MakeAspect<LoggingAspect>(p.get())->g();

cout << endl;
X object;
MakeAspect<LoggingAspectRaw>(&object)->test_echo();

auto proxy = MakeAspect<LoggingAspectRaw>(&object);
proxy->test_echo();
proxy->g();
proxy->test_param("中文!!!");

MakeAspect<LoggingAspect>(p.get())->g();
}



*/

#endif
