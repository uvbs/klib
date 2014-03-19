
#include <string>

#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

class Echo
{
public:
    std::string echo(const std::string &s) { return s; }
};

RCF_BEGIN(I_Echo, "I_Echo")
    RCF_METHOD_R1(std::string, echo, const std::string &)
RCF_END(I_Echo)


class Client
{
public:

    Client(const std::string &ip, int port) : ip(ip), port(port)
    {}

    void operator()()
    {
        try
        {
            RcfClient<I_Echo> echo(ip, port);
            std::string s0 = "something special";
            for (int i=0;i<10; i++)
            {
                try
                {
                    std::string s = echo.echo(s0); 
                    BOOST_CHECK(s0 == s);
                }
                catch(const RCF::Exception &e)
                {
                    RCF_TRACE("")(e); // since we're overloading the server there will be some exceptions
                }
            }
        }
        catch(...)
        {
            BOOST_CHECK(1==0);
        }
    }

private:
    std::string ip;
    int port;
};

int test_main(int argc, char **argv)
{
    util::CommandLineOption<int> port( "port", util::Ports::getNext(), "port number" );
    util::CommandLine::getSingleton().parse(argc, argv);
  
    RCF::RcfServer server(port);
    server.bind<I_Echo, Echo>();
    server.start();

    Platform::Threads::thread_group clients;
    for (int i=0; i<100; i++)
    {
        clients.create_thread( Client("localhost", port) );
    }
    clients.join_all();
    
    return boost::exit_success;
}


