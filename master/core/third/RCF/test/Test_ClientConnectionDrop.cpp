
#include <memory>
#include <string>

#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

RCF_BEGIN(I_X, "I_X")
    RCF_METHOD_R1(std::string, echo, std::string)
RCF_END(I_X)

struct X { std::string echo(const std::string &s) { return s; } };

int test_main(int argc, char **argv)
{

    util::CommandLineOption<int> port( "port", util::Ports::getNext(), "port number" );
    util::CommandLine::getSingleton().parse(argc, argv);

    X x;
    std::string s; 

    std::auto_ptr<RCF::RcfServer> pServer( new RCF::RcfServer(port) );
    pServer->bind<I_X>(x);
    pServer->start();

    RcfClient<I_X> client("localhost", port);
    s = client.echo("abc");
    BOOST_CHECK( s == "abc" );
    s = client.echo("def");
    BOOST_CHECK( s == "def" );

    pServer->stop();
    pServer->start();
    s = client.echo("abc");
    BOOST_CHECK( s == "abc" );
    s = client.echo("def");
    BOOST_CHECK( s == "def" );

#ifdef __WINDOWS__
    // following tests will only work on Windows,
    // since the Unix sockets linger for a while after they're closed.
    pServer->reset();
    pServer->start();
    s = client.echo("abc");
    BOOST_CHECK( s == "abc" );
    s = client.echo("def");
    BOOST_CHECK( s == "def" );

    pServer.reset( new RCF::RcfServer(port) );
    pServer->bind<I_X>(x);
    pServer->start();
    s = client.echo("abc");
    BOOST_CHECK( s == "abc" );
    s = client.echo("def");
    BOOST_CHECK( s == "def" );
#endif // __WINDOWS__

    return 0;
}