
#include <string>

#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/ServerInterfaces.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

int test_main(int argc, char **argv)
{
    util::CommandLineOption<std::string> ip("ip", "localhost", "endpoint server ip");
    util::CommandLineOption<int> port("port", util::Ports::getCurrent(), "endpoint server port number");
    util::CommandLineOption<std::string> endpoint("endpoint", "MyEndpoint", "endpoint name");
    util::CommandLine::getSingleton().parse(argc, argv);

    RCF::RcfClient<RCF::I_ServerInfo> serverInfoA(ip, port, endpoint);
    std::string name = serverInfoA.getName();
    BOOST_CHECK(name == endpoint.get());

    return 0;
}
