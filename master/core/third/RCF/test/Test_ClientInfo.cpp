
#include <string>

#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/SessionInfo.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

class GetMyIp
{
public:
    std::string getMyIp() 
    { 
        RCF::ClientInfo clientInfo = RCF::getCurrentSessionInfo().getClientInfo();
        std::string ipAddr = clientInfo.getAddress();
        sockaddr_in addr = clientInfo.getSockAddr();
        return ipAddr; 
    }
};

RCF_BEGIN(I_GetMyIp, "I_GetMyIp")
    RCF_METHOD_R0(std::string, getMyIp)
RCF_END(I_GetMyIp)

int test_main(int argc, char **argv)
{

    util::CommandLineOption<int> port( "port", util::Ports::getNext(), "port number" );
    util::CommandLine::getSingleton().parse(argc, argv);

    RCF::RcfServer server(port);
    server.bind<I_GetMyIp, GetMyIp>();
    server.start();

    std::string myIp = RcfClient<I_GetMyIp>("localhost", port).getMyIp();
    BOOST_CHECK(myIp == "127.0.0.1");
    
    return boost::exit_success;
}


