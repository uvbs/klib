
#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

int test_main(int argc, char **argv)
{
    util::CommandLineOption<std::string> ip("ip", "localhost", "endpoint server ip");
    util::CommandLineOption<int> port("port", util::Ports::getCurrent(), "endpoint server port number");
    util::CommandLineOption<std::string> endpoint("endpoint", "MyEndpoint", "endpoint name");
    util::CommandLine::getSingleton().parse(argc, argv);

    RCF::RcfServer server(ip, port, endpoint);
    server.start();

    return 0;
}
