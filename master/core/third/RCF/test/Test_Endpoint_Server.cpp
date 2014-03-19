
#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

int test_main(int argc, char **argv)
{
    util::CommandLineOption<int> port("port", util::Ports::getNext(), "endpoint server port number");
    util::CommandLine::getSingleton().parse(argc, argv);

    RCF::RcfServer server(port);
    server.start();

    return 0;
}
