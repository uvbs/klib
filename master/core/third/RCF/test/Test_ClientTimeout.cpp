
#include <iostream>
#include <sstream>
#include <string>

#include <boost/test/minimal.hpp>
//#define BOOST_CHECK(arg)
//namespace boost { int exit_success = 0; }

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

#include <SF/memory.hpp>

// Need to disable broken pipe signals for OS's such as Solaris
#ifdef SIGPIPE
#include <signal.h>
#include <RCF/util/InitDeinit.hpp>
UTIL_ON_INIT( sigignore(SIGPIPE) )
#endif

RCF_BEGIN(I_X, "I_X")
    RCF_METHOD_R1(std::string, echo, std::string);
    RCF_METHOD_V1(void, dummy, std::string);
RCF_END(I_X);

class X
{
public:
    std::string echo(const std::string &s)
    {
        // wait 2s and then echo the string
        Platform::OS::Sleep(2);
        return s;
    }
    void dummy(const std::string &s)
    {
        // wait 2s and then return
        Platform::OS::Sleep(2);
        return;
    }
};

// Note: these tests fail on Borland C++ because the scopeguards that are supposed
// to reset the client stub connections are not being triggered.

int test_main(int argc, char **argv)
{
    util::CommandLineOption<int> port("port", util::Ports::getNext(), "port number");
    util::CommandLine::getSingleton().parse(argc, argv);

    std::string s;

    RCF::RcfServer server(port);
    server.bind<I_X,X>();
    server.start();

    RcfClient<I_X> client("localhost", port);

    // this call will timeout
    client.setRecvTimeoutMs(1000);
    try 
    {
        std::cout << "A\n";
        s = client.echo("abc");
        BOOST_CHECK(1==0);
    }
    catch (const RCF::Exception &e)
    {
        BOOST_CHECK(1==1);
        std::cout << e.what();
    }
    
    // these ones won't
    client.setRecvTimeoutMs(15000);
    std::cout << "B\n";
    s = client.echo("def");
    BOOST_CHECK(s == "def");

    std::cout << "C\n";
    client.dummy("abc");

    // this call will timeout
    client.setRecvTimeoutMs(1000);
    try 
    {
        std::cout << "D\n";
        client.dummy("abc");
        BOOST_CHECK(1==0);
    }
    catch (const RCF::Exception &e)
    {
        BOOST_CHECK(1==1);
        std::cout << e.what();
    }

    // these ones won't
    client.setRecvTimeoutMs(15000);
    
    std::cout << "E\n";
    s = client.echo("def");
    BOOST_CHECK(s == "def");

    std::cout << "F\n";
    client.dummy("abc");

    return boost::exit_success;
}
