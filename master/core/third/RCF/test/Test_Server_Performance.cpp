
#include <string>

#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

#include "Test_Performance.hpp"

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

