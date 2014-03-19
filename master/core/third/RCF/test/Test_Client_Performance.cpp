#include <sstream>

#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

#include "Test_Performance.hpp"

int test_main(int argc, char **argv)
{
    util::CommandLineOption<std::string> ip("ip", "localhost", "ip");
    util::CommandLineOption<int> port("port", util::Ports::getCurrent(), "port number" );
    util::CommandLineOption<int> calls( "calls", 100, "number of calls");
    util::CommandLineOption<int> protocol("protocol", 0, "serialization protocol: 1, 2, 3 or 4");
    util::CommandLine::getSingleton().parse(argc, argv);

    runClientTests(ip, port, calls, protocol);
    
    return boost::exit_success;
}

