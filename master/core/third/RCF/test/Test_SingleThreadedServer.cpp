
#include <iostream>
#include <memory>
#include <string>

#include <boost/bind.hpp>
#include <boost/test/minimal.hpp>
#include <boost/thread/thread.hpp>

#ifdef RCF_USE_BOOST_THREADS
#error This test must be built with RCF in singlethreaded mode!
#endif

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

RCF_BEGIN(I_X, "I_X")
    RCF_METHOD_R1(std::string, echo, std::string)
RCF_END(I_X)

struct X { std::string echo(const std::string &s) { return s; } };

std::auto_ptr<RCF::RcfServer> server;

struct ServerThread
{
    void operator()()
    {
        server->start();
    }
};

bool serverStarted = false;

template<typename T, typename U>
class A
{
public:
    void onServerStarted(RCF::RcfServer &server)
    {
        serverStarted = true;
    }
};

void onServerStarted(RCF::RcfServer &server)
{
    serverStarted = true;
}

int test_main(int argc, char **argv)
{
    util::CommandLineOption<int> port( "port", util::Ports::getNext(), "port number" );
    util::CommandLine::getSingleton().parse(argc, argv);

    server.reset(new RCF::RcfServer(port));
    server->bind<I_X,X>();

    // callback to a standalone function...
    server->setStartCallback(onServerStarted );
    
    // or to a member function
    A<int,int> a;
    server->setStartCallback(&A<int,int>::onServerStarted, a); // on some compilers (msvc 7.1 at least), the "&" isn't needed
    
    boost::thread serverThread( (0,ServerThread()) );

    Platform::OS::Sleep(1);

    RcfClient<I_X> client("localhost", port);
    std::string s = client.echo("abc");
    BOOST_CHECK(s == "abc");
    BOOST_CHECK(serverStarted == true);

    server->stop();
    serverThread.join();

    server.reset();
    return 0;
}

