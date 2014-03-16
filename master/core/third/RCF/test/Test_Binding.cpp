
#include <memory>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>


class Echo
{
public:
    std::string echo(const std::string &s) { return s; }
};

RCF_BEGIN(I_Echo, "I_Echo")
    RCF_METHOD_R1(std::string, echo, const std::string &)
RCF_END(I_Echo)


int test_main(int argc, char **argv)
{

    int port = util::Ports::getNext();

    std::string s0 = "whatever";

    {
        RCF::RcfServer server(port);
        server.bind<I_Echo, Echo>();
        server.start();

        RcfClient<I_Echo> echo("localhost", port);
        std::string s = echo.echo(s0);
        BOOST_CHECK(s == s0);
    }

    port = util::Ports::getNext();

    {   
        RCF::RcfServer server(port);
        server.bind<I_Echo, Echo>(true);
        server.start();

        RcfClient<I_Echo> echo("localhost", port);
        std::string s = echo.echo(s0);
        BOOST_CHECK(s == s0);
    }

    port = util::Ports::getNext();

    {
        Echo e;
        RCF::RcfServer server(port);
        server.bind<I_Echo>(e);
        server.start();

        RcfClient<I_Echo> echo("localhost", port);
        std::string s = echo.echo(s0);
        BOOST_CHECK(s == s0);
    }

    port = util::Ports::getNext();

    {
        std::auto_ptr<Echo> e( new Echo );
        RCF::RcfServer server(port);
        server.bind<I_Echo, Echo>(e);
        server.start();

        RcfClient<I_Echo> echo("localhost", port);
        std::string s = echo.echo(s0);
        BOOST_CHECK(s == s0);
    }

    port = util::Ports::getNext();

    {
        boost::shared_ptr<Echo> e( new Echo );
        RCF::RcfServer server(port);
        server.bind<I_Echo, Echo>(e);
        server.start();

        RcfClient<I_Echo> echo("localhost", port);
        std::string s = echo.echo(s0);
        BOOST_CHECK(s == s0);
    }

    return boost::exit_success;
}

