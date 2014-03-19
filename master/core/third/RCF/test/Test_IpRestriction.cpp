
#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

// Need to disable broken pipe signals for OS's such as Solaris
#ifdef SIGPIPE
#include <signal.h>
#include <RCF/util/InitDeinit.hpp>
static void setSigignore() { sigignore(SIGPIPE); }
UTIL_ON_INIT( sigignore(SIGPIPE) )
#endif


RCF_BEGIN(I_X, "I_X")
    RCF_METHOD_V0(void, f);
RCF_END(I_X);

class X
{
public:
    void f() {}
};

int test_main(int argc, char **argv)
{
    //util::CommandLineOption<int> port("port", util::Ports::getNext(), "port number");
    util::CommandLine::getSingleton().parse(argc, argv);

    int port = 0;

    std::vector<std::string> ips;
    ips.push_back("www.usatoday.com");
    ips.push_back("www.sunet.se");
    ips.push_back("www.google.com");

    X x;
    {
        port = util::Ports::getNext();
        RCF::RcfServer server(port);
        server.setAllowedClientIps(ips);
        server.bind<I_X>(x);
        server.start();

        try 
        {
            RcfClient<I_X> client("localhost", port);
            client.f();
            BOOST_CHECK(1==0);
        }
        catch(const RCF::Exception &e)
        {
            RCF_TRACE("")(e);
            BOOST_CHECK(1==1);
        }

        ips = server.getAllowedClientIps();
        ips.push_back("localhost");
        server.setAllowedClientIps(ips);

        RcfClient<I_X> client("localhost", port);
        client.f();
    }

    // test interface restriction

    // loopback interface
    std::vector<std::string> interfaces;
    interfaces.push_back("127.0.0.1");
    interfaces.push_back("localhost");
    
    for (unsigned int i=0; i<interfaces.size(); i++)
    {
        port = util::Ports::getNext();
        RCF::RcfServer server(port);
        server.bind<I_X>(x);
        server.setNetworkInterface(interfaces[i]);
        server.start();

        RcfClient<I_X>("localhost", port).f();
        BOOST_CHECK(1==1);
    }
    
    // erroneous interface
    {
        port = util::Ports::getNext();
        RCF::RcfServer server(port);
        server.bind<I_X>(x);
        server.setNetworkInterface("1.2.3.4");
        try 
        {
            server.start();
            BOOST_CHECK(1==0);
        }
        catch (const RCF::Exception &e)
        {
            BOOST_CHECK(1==1);
            RCF_TRACE("")(e);
        }
    }

    return boost::exit_success;
}
