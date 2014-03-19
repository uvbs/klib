
#include <string>

#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

RCF_BEGIN(I_X, "I_X")
    RCF_METHOD_R1(std::string, echo, const std::string &);
RCF_END(I_X);

int test_main(int argc, char **argv)
{
    util::CommandLineOption<std::string> ip("ip", "localhost", "ip");
    util::CommandLineOption<int> port("port", util::Ports::getCurrent(), "port number" );
    util::CommandLine::getSingleton().parse(argc, argv);

    std::string s0 = "something special";
    std::string s = RcfClient<I_X>(ip, port).echo(s0);
    BOOST_CHECK(s == s0);

    return boost::exit_success;
}

