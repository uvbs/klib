
#include <string>

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
    util::CommandLineOption<int> port( "port", util::Ports::getNext(), "port number" );
    util::CommandLine::getSingleton().parse(argc, argv);
  
    RCF::RcfServer server(port);
    server.bind<I_Echo, Echo>();
    server.start();

    RcfClient<I_Echo> echo("localhost", port);
    std::string s0 = "something special";
    for (int i=0;i<100; i++)
    {
        std::string s = echo.echo(s0); 
        BOOST_CHECK(s0 == s);
    }
    
    return boost::exit_success;
}
