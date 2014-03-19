
#ifndef _TEST_PERFORMANCE_HPP_
#define _TEST_PERFORMANCE_HPP_

#include <string>

#include <boost/lexical_cast.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/Profile.hpp>

class X 
{
public:
    void f0() {}
    int f1(int) { return 0; }
    std::string f2(const std::string &) { return ""; }
};

RCF_BEGIN( I_X, "I_X" )
    RCF_METHOD_V0(void, f0)
    RCF_METHOD_R1(int, f1, int)
    RCF_METHOD_R1(std::string, f2, const std::string &)
RCF_END( I_X )

void runClientTests(const std::string &ip, int port, int calls, int protocol)
{
    RcfClient<I_X> client(ip, port);
    client.f0();
    client.f1(1);
    client.f2("");

    for (int p=1; p<=10; p++)
    {
        if (protocol == p || (protocol == 0 && RCF::isProtocolSupported(p)))
        {
            client.setProtocol(p);
            std::string protocolName = RCF::getProtocolName(p);
            std::string titleHeader = 
                "Protocol: " + protocolName + 
                ", Calls: " + boost::lexical_cast<std::string>(calls);
            {
                std::string title = titleHeader + ", void f()";
                util::Profile profile(title);
                for (int i=0; i<calls; i++)
                {
                    client.f0();
                }
            }
            {
                std::string title = titleHeader + ", int f(int)";
                util::Profile profile(title);
                for (int i=0; i<calls; i++)
                {
                    client.f1(0);
                }
            }
            {
                std::string title = titleHeader + ", std::string f(std::string)";
                util::Profile profile(title);
                for (int i=0; i<calls; i++)
                {
                    client.f2("");
                }
            }
        }
    }
}

#endif // ! _TEST_PERFORMANCE_HPP_
