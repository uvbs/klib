
#include <string>

#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

#include <SF/vector.hpp>

class TImage
{
public:
    int rows;
    int cols;
    std::vector<unsigned char> red;
    std::vector<unsigned char> green;
    std::vector<unsigned char> blue;

    template<typename Archive>
    void serialize(Archive &archive, unsigned int version)
    {
        archive & rows & cols & red & green & blue;
    }
};

typedef std::vector<unsigned char> Channel;

RCF_BEGIN(RemoteLogger, "RemoteLogger")
    RCF_METHOD_V1(void, image, const TImage&);
    RCF_METHOD_V3(void, getImage, const Channel&,const Channel&,const Channel&);
RCF_END(RemoteLogger);

RCF_BEGIN(I_Echo, "I_Echo")
    RCF_METHOD_R1(std::string, echo, const std::string &)
RCF_END(I_Echo)


class ServerImpl
{
public:
    std::string echo(const std::string &s) 
    { 
        return s; 
    }

    void image(const TImage &t)
    {
        int count = t.red.size();
    }

    void getImage(const Channel &red, const Channel &green, const Channel &blue)
    {
        int count = red.size();
    }
};

class Dummy
{
public:
    int n;
    std::string s;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int)
    {
        ar & n & s;
    }

};

void save()
{
    Dummy dummy;
    dummy.n = 17;
    dummy.s = "seventeen";

    // Save function
    // other stuff before

    char *strFileName = "dummy.bin";

    std::ofstream out_file(strFileName, std::ios::out);
    //SF::OBinaryStream out_stream(out_file);
    SF::OTextStream out_stream(out_file);
    try
    {
        //out_stream << tempPlanche.m_plancheOpt;
        out_stream << dummy;
    }
    catch(SF::Exception ex)
    {
        //AfxMessageBox(ex.what());
    }
    catch(...)
    {
        // handle error...
    }
    out_file.flush();
    out_file.close();

}

void load()
{

    // Load function (in another part of the code where it is needed)
    char *strFileName = "dummy.bin";
    //CPlanche planche;
    Dummy dummy;
    std::ifstream in_file(strFileName, std::ios::in);
    //SF::IBinaryStream in_stream(in_file);
    SF::ITextStream in_stream(in_file);
    try
    {
        in_stream >> dummy;
        //in_stream >> planche;
        //pDoc->JouePlanche(planche);
    }
    catch(SF::Exception ex)
    {
        //AfxMessageBox(ex.what());
    }
    catch(...)
    {
        // Handle error
    }
    in_file.close();

}

int test_main(int argc, char **argv)
{
    util::CommandLineOption<int> port( "port", util::Ports::getNext(), "port number" );
    util::CommandLine::getSingleton().parse(argc, argv);

    RCF::getRcfTraceChannel().setTraceTarget("ODS");

    save();
    load();

    ServerImpl serverImpl;

    RCF::RcfServer server(port);
    server.bind<I_Echo>(serverImpl);
    server.bind<RemoteLogger>(serverImpl);
    server.start();

    RcfClient<I_Echo> echo("localhost", port);
    std::string s0 = "something special";
    for (int i=0;i<100; i++)
    {
        std::string s = echo.echo(s0); 
        BOOST_CHECK(s0 == s);
    }

    bool flag = true;

    if (flag)
    {
        TImage t;
        t.rows = 10;
        t.cols = 15;
        if (flag)
        {
            int count = 200;
            for (int i=0; i<count; ++i)
            {
                t.red.push_back(0xff);
                t.green.push_back(0xff);
                t.blue.push_back(0xff);
            }
        }
        RcfClient<RemoteLogger> client("localhost", port);
        client.image(t);
        client.image(t);
        client.image(t);
    }

    if (flag)
    {
        Channel red, green, blue;
        int count = 195;
        if (flag)
        {
            count = 200;
        }
        for (int i=0; i<count; ++i)
        {
            red.push_back('r');
            green.push_back('g');
            blue.push_back('b');
        }
        
        RcfClient<RemoteLogger> client("localhost", port);
        client.getImage(red, green, blue);
        client.getImage(red, green, blue);
        client.getImage(red, green, blue);
    }

    return boost::exit_success;
}
