
#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

namespace A {

    RCF_BEGIN( Calculator, "Calculator" )
        RCF_METHOD_R2(double, add, double, double)
        RCF_METHOD_R2(double, sub, double, double)
        RCF_METHOD_R2(double, mul, double, double)
        RCF_METHOD_R2(double, div, double, double)
        RCF_METHOD_V3(void, add, double, double, double&)
        RCF_METHOD_V3(void, sub, double, double, double&)
        RCF_METHOD_V3(void, mul, double, double, double&)
        RCF_METHOD_V3(void, div, double, double, double&)
        RCF_METHOD_V1(void, M_in, double)
        RCF_METHOD_V1(void, M_plus, double)
        RCF_METHOD_R0(double, RcM)
        RCF_METHOD_V1(void, RcM, double&)
    RCF_END( Calculator )

    RCF_BEGIN( X, "X" )
        RCF_METHOD_R0(int, f0)
        RCF_METHOD_R1(int, f1, int)
        RCF_METHOD_R2(int, f2, int, int)
        RCF_METHOD_R3(int, f3, int, int, int)
        RCF_METHOD_R4(int, f4, int, int, int, int)
        RCF_METHOD_R5(int, f5, int, int, int, int, int)
        RCF_METHOD_V0(void, f6)
        RCF_METHOD_V1(void, f7, int)
        RCF_METHOD_V2(void, f8, int, int)
        RCF_METHOD_V3(void, f9, int, int, int)
        RCF_METHOD_V4(void, f10, int, int, int, int)
        RCF_METHOD_V5(void, f11, int, int, int, int, int)
    RCF_END( X )


} //namespace A


namespace A {
namespace B {

class CalculatorDivideByZeroException : public std::exception {
public:
    CalculatorDivideByZeroException(std::string msg) : msg_(msg) {}
    ~CalculatorDivideByZeroException() throw() {}
    const char *what() const throw() { return msg_.c_str(); }
private:
    std::string msg_;
};

class Calculator {
public:
    Calculator() : M_(0) {}

    double add(double x, double y)        { return x+y; } 
    double sub(double x, double y)        { return x-y; }
    double mul(double x, double y)        { return x*y; }    
    double div(double x, double y)        { if (y==0) throw CalculatorDivideByZeroException("Divide by zero"); return x/y; }

    void add(double x, double y, double &z)    { z = x+y; }
    void sub(double x, double y, double &z)    { z = x-y; }
    void mul(double x, double y, double &z)    { z = x*y; }
    void div(double x, double y, double &z)    { if (y==0) throw int(5); z = x/y; }

    void M_in(double x)            { M_ = x; }
    void M_plus(double x)            { M_ += x; }
    double RcM()                { return M_; }
    void RcM(double &x)            { x = M_; }

private:
    Calculator(const Calculator &);
    Calculator &operator=(const Calculator &);
    double M_;
};

class X {
public:
    int f0() { return 1; }
    int f1(int) { return 1; }
    int f2(int, int) { return 1; }
    int f3(int, int, int) { return 1; }
    int f4(int, int, int, int) { return 1; }
    int f5(int, int, int, int, int) { return 1; }
    void f6() {}
    void f7(int) {}
    void f8(int, int) {}
    void f9(int, int, int) {}
    void f10(int, int, int, int) {}
    void f11(int, int, int, int, int) {}
};


} // namespace B
} // namespace A

inline bool fuzzy_eq(double d1, double d2, float tolerance = .1) 
{
    return (d1-d2)*(d1-d2) < tolerance;
}

void testCalculatorClient(const std::string &ip, int port)
{

    using namespace A;
    
    RcfClient<Calculator> calc(ip, port);
    RcfClient<Calculator> calc2 = calc;
    RcfClient<Calculator> calc3;
    calc3 = calc;
    RcfClient<Calculator> calc4(ip, port);
    
    double ret = 0;
    double x = 0;
    double y = 0;
    
    x = 1.5;
    y = 2.5;
    ret = calc.add(x,y); BOOST_CHECK( fuzzy_eq(ret, x+y) );
    ret = calc.sub(x,y); BOOST_CHECK( fuzzy_eq(ret, x-y) );
    ret = calc.mul(x,y); BOOST_CHECK( fuzzy_eq(ret, x*y) );
    ret = calc.div(x,y); BOOST_CHECK( fuzzy_eq(ret, x/y) );

    x = 1.5;
    y = 2.5;
    ret = calc2.add(x,y); BOOST_CHECK( fuzzy_eq(ret, x+y) );
    ret = calc2.sub(x,y); BOOST_CHECK( fuzzy_eq(ret, x-y) );
    ret = calc2.mul(x,y); BOOST_CHECK( fuzzy_eq(ret, x*y) );
    ret = calc2.div(x,y); BOOST_CHECK( fuzzy_eq(ret, x/y) );

    x = 1.5;
    y = 2.5;
    ret = calc3.add(x,y); BOOST_CHECK( fuzzy_eq(ret, x+y) );
    ret = calc3.sub(x,y); BOOST_CHECK( fuzzy_eq(ret, x-y) );
    ret = calc3.mul(x,y); BOOST_CHECK( fuzzy_eq(ret, x*y) );
    ret = calc3.div(x,y); BOOST_CHECK( fuzzy_eq(ret, x/y) );

    x = 1.5;
    y = 2.5;
    ret = calc4.add(x,y); BOOST_CHECK( fuzzy_eq(ret, x+y) );
    ret = calc4.sub(x,y); BOOST_CHECK( fuzzy_eq(ret, x-y) );
    ret = calc4.mul(x,y); BOOST_CHECK( fuzzy_eq(ret, x*y) );
    ret = calc4.div(x,y); BOOST_CHECK( fuzzy_eq(ret, x/y) );

    x = 3.14;
    y = -2.718;
    calc4.add(x,y, ret); BOOST_CHECK( fuzzy_eq(ret, x+y) );
    calc4.sub(x,y, ret); BOOST_CHECK( fuzzy_eq(ret, x-y) );
    calc4.mul(x,y, ret); BOOST_CHECK( fuzzy_eq(ret, x*y) );
    calc4.div(x,y, ret); BOOST_CHECK( fuzzy_eq(ret, x/y) );

    x = 1;
    y = 2;
    ret = 1;
    ret = calc4.RcM(); BOOST_CHECK( fuzzy_eq(ret, 0) );
    calc4.M_in(x);
    calc4.M_plus(y);
    calc4.RcM(ret); BOOST_CHECK( fuzzy_eq(ret, x+y) );
    calc4.M_in(0);
    ret = calc4.RcM(); BOOST_CHECK( fuzzy_eq(ret, 0) );

    {
        RcfClient<X> x(ip, port);
        x.f0();
        x.f1(1);
        x.f2(1,1);
        x.f3(1,1,1);
        x.f4(1,1,1,1);
        x.f5(1,1,1,1,1);
        x.f6();
        x.f7(1);
        x.f8(1,1);
        x.f9(1,1,1);
        x.f10(1,1,1,1);
        x.f11(1,1,1,1,1);
    }

    x = 1;
    y = 1;

    ret = 0;
    ret = calc4.add(1,1); BOOST_CHECK( fuzzy_eq(ret, 2) );
    ret = calc4.add(1,1); BOOST_CHECK( fuzzy_eq(ret, 2) );

    ret = 0;
    calc4.setOneway(true);
    ret = calc4.add(1,1); BOOST_CHECK( fuzzy_eq(ret, 0) );
    ret = calc4.add(1,1); BOOST_CHECK( fuzzy_eq(ret, 0) );
    ret = calc4.add(1,1); BOOST_CHECK( fuzzy_eq(ret, 0) );

    ret = 0;
    calc4.setOneway(false);
    ret = calc4.add(1,1); BOOST_CHECK( fuzzy_eq(ret, 2) );
    ret = calc4.add(1,1); BOOST_CHECK( fuzzy_eq(ret, 2) );
    

    // Test propagation of remote std::exception derived exeptions
    x = 1;
    y = 0;
    try 
    { 
      ret = calc.div(x,y); 
      BOOST_CHECK(1==0); 
    } 
    catch(const RCF::RemoteException &e)
    {
        RCF_TRACE("RCF::RemoteException caught")(e);
        BOOST_CHECK(1==1);
    }
    

#ifndef __BORLANDC__ // Scopeguard bug with Borland C++... causes theses tests to crash

    // Test propagation of remote non-std::exception derived exeptions
    x = 1;
    y = 0;
    ret = 0;
    try 
    { 
        calc.div(x,y, ret); 
        BOOST_CHECK(1==0); 
    } 
    catch(const RCF::RemoteException &e)
    {
        RCF_TRACE("RCF::RemoteException caught")(e);
        BOOST_CHECK(1==1);
    }

#endif //! __BORLANDC__

}

class Client
{
public:

    Client(const std::string &ip, int port, int n) :
        ip(ip),
        port(port),
        n(n)
    {}

    void operator()()
    {
        try 
        {
            for (int i=0; i<n; i++)
            {
                testCalculatorClient(ip, port);
            }
        }
        catch(...)
        {
            BOOST_CHECK(1==0);
        }
    }

private:
    std::string ip;
    int port;
    int n;
};


int test_main(int argc, char **argv)
{
    util::CommandLineOption<int> port( "port", util::Ports::getNext(), "port number" );
    util::CommandLine::getSingleton().parse(argc, argv);

    RCF::RcfServer server(port);
    server.bind<A::Calculator, A::B::Calculator>();
    server.bind<A::X, A::B::X>();
    server.start();

    Platform::Threads::thread_group clients;
    clients.create_thread( Client("localhost", port, 2) );
    clients.create_thread( Client("localhost", port, 2) );
    clients.create_thread( Client("localhost", port, 2) );
    clients.join_all();
    
    server.stop();

    return boost::exit_success;
}



