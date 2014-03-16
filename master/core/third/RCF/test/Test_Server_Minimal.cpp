
#include <string>

#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

class X
{
public:
    std::string echo(const std::string &s) { return s; }
};

RCF_BEGIN(I_X, "I_X")
    RCF_METHOD_R1(std::string, echo, const std::string &);
RCF_END(I_X);

int test_main(int argc, char **argv)
{
    util::CommandLineOption<int> port( "port", util::Ports::getNext(), "port number" );
    util::CommandLine::getSingleton().parse(argc, argv);

    X x;
    RCF::RcfServer server(port);
    server.bind<I_X>(x);
    server.start();

    return boost::exit_success;
}

