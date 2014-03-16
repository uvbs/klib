
#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/ServerInterfaces.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>


class EndpointClient
{
public:

    EndpointClient(const std::string &ip, int port, const std::string &endpointName, int n) :
      ip(ip),
          port(port),
          endpointName(endpointName),
          n(n)
      {}

      void operator()()
      {
          for (int i=0; i<n; i++)
          {
              try
              {
                  RCF::RcfClient<RCF::I_ServerInfo> serverInfoA(ip, port, endpointName);
                  std::string name = serverInfoA.getName();
                  BOOST_CHECK(name == endpointName);
              }
              catch (const RCF::RemoteException &e)
              {
                  RCF_TRACE("Caught remote exception")(e);
                  unsigned int seconds = 3;
                  RCF_TRACE("Sleeping...")(seconds);
                  Platform::OS::Sleep(seconds);
              }
              catch(const std::exception &e)
              {
                  BOOST_CHECK( 1==0 );
                  RCF_TRACE("Caught std::exception")(e);
                  unsigned int seconds = 3;
                  RCF_TRACE("Sleeping...")(seconds);
                  Platform::OS::Sleep(seconds);
              }
              catch(...)
              {
                  BOOST_CHECK( 1==0 );
                  RCF_TRACE("Caught something nasty");
                  return;
              }
          }
      }

private:
    std::string ip;
    int port;
    std::string endpointName;
    int n;
};

int test_main(int argc, char **argv)
{
    util::CommandLineOption<int> port( "port", util::Ports::getNext(), "port number" );
    util::CommandLine::getSingleton().parse(argc, argv);

    std::string ip = "localhost";
    std::string endpointA = "EndpointA";
    std::string endpointB = "EndpointB";
    std::string endpointC = "EndpointC";

    RCF::RcfServer server(port);
    server.start();

    {
        Platform::OS::Sleep(1);

        RCF::RcfServer serverA(ip, port, endpointA);
        serverA.start();

        RCF::RcfServer serverB(ip, port, endpointB);
        serverB.start();

        RCF::RcfServer serverC(ip, port, endpointC);
        serverC.start();

        Platform::Threads::thread_group endpointClients;
        endpointClients.create_thread( EndpointClient(ip, port, endpointA, 10) );
        endpointClients.create_thread( EndpointClient(ip, port, endpointB, 10) );
        endpointClients.create_thread( EndpointClient(ip, port, endpointC, 10) );
        endpointClients.join_all();
    }

    return boost::exit_success;

}

