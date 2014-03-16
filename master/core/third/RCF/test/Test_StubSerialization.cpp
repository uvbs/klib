
#include <iostream>
#include <sstream>
#include <string>

#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

#include <SF/memory.hpp>

RCF_BEGIN(I_X, "I_X")
    RCF_METHOD_V0(void, increment)
    RCF_METHOD_R0(int, getCount)
RCF_END(I_X);

// gcc < 3.4 doesn't support boost::is_abstract so we have to be explicit
//#if (__GNUC__ < 3 || (__GNUC__ == 3 && __GNUC_MINOR__ < 4))
namespace boost {
    template<>
    struct is_abstract< RcfClient<I_X> >
    {
        enum { value = false };
        typedef boost::mpl::false_ type;
    };
    template<>
    struct is_abstract< const RcfClient<I_X> >
    {
        enum { value = false };
        typedef boost::mpl::false_ type;
    };
}
//#endif

RCF_BEGIN(I_Y, "I_Y")
    RCF_METHOD_R1( RcfClient<I_X>, pass, const RcfClient<I_X> & )
RCF_END(I_Y)



class X
{
public:
    X() : count()
    {}

    void increment()
    {
        count++;
    }

    int getCount()
    {
        return count;
    }

private:
    int count;
};

class Y
{
public:
    RcfClient<I_X> pass(RcfClient<I_X> stub)
    {
        stub.increment();
        return stub;
    }
};

int test_main(int argc, char **argv)
{
    util::CommandLineOption<int> port("port", util::Ports::getNext(), "port number");
    util::CommandLine::getSingleton().parse(argc, argv);

    {
        // test serialize and deserialize a stub between invocations

        RCF::RcfServer server(port);
        server.bind<I_X,X>();
        server.start();

        RcfClient<I_X> stub1("localhost", port);
        stub1.increment();
        std::ostringstream os;
        SF::OTextStream(os) << stub1;
        std::string s = os.str();

        {
            RcfClient<I_X> stub2;
            std::istringstream is(s);
            SF::ITextStream(is) >> stub2;
            stub2.increment();
        }
        {
            RcfClient<I_X> stub3;
            std::istringstream is(s);
            SF::ITextStream(is) >> stub3;
            stub3.increment();
        }

        int count = stub1.getCount();
        BOOST_CHECK(count == 3);
    }

    {
        // distributed test, passing client stubs from server to server
        int portA = util::Ports::getNext();
        int portB = util::Ports::getNext();
        
        RCF::RcfServer serverA(portA);
        serverA.bind<I_X, X>();
        serverA.start();

        RCF::RcfServer serverB(portB);
        serverB.bind<I_Y, Y>();
        serverB.start();

        RcfClient<I_X> stub1("localhost", portA);
        stub1.increment();
        RcfClient<I_Y> stub2("localhost", portB);
        RcfClient<I_X> stub3 = stub2.pass(stub1);
        stub1.increment();
        stub3.increment();

        int count = stub3.getCount();
        BOOST_CHECK(count == 4);
    }

    return boost::exit_success;
}
