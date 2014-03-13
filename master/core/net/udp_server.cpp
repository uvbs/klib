#include "udp_server.h"

namespace klib {
namespace net {


void udp_server::start()
{
    socket_.sign_msg_recv.connect(this, &udp_server::on_udp_msg);
    socket_.start_async();

    this->register_dispatcher_();
}

void udp_server::on_udp_msg(udp_socket*, ip_v4 addr, USHORT port, char* buff, int ilen)
{
    net_archive ar(buff, ilen);
    cmd_header header;
    BOOL ret;

    ar >> header;
    if (!ar.get_error()) 
    {
        fdDispatchFunction f;
        if (dispatcher_.find_dispatcher(header.cmd, f))
        {
            f(addr, port, &header, &ar, ret);
        }
    }
    else
    {

    }
}

void udp_server::register_dispatcher_()	
{

}


}}