
#include <boost/lexical_cast.hpp>
#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

#include "Test_Performance.hpp"

namespace util {
    InitCallback initCallbackStaticArray[InitCallbackMaxCount];
    DeinitCallback deinitCallbackStaticArray[DeinitCallbackMaxCount];
    DeinitCallback *deinitCallbackAllocatedArray;
    bool initCallbacksInvoked;
}

int test_main(int argc, char **argv)
{

    util::CommandLineOption<int> port( "port", util::Ports::getNext(), "port number" );
    util::CommandLineOption<int> calls( "calls", 100, "number of calls");
    util::CommandLineOption<int> protocol("protocol", 0, "serialization protocol: 1, 2, 3 or 4");
    util::CommandLine::getSingleton().parse(argc, argv);

    X x;
    RCF::RcfServer server(port);
    server.bind<I_X>(x);
    server.start();

    runClientTests("localhost", port, calls, protocol);

    return boost::exit_success;
}

