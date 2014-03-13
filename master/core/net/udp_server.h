
#pragma once

#include "udp_socket.h"
#include "proto/dispatcher.h"
#include "proto/net_archive.h"
#include "proto/command_header.h"


namespace klib{
namespace net{

typedef std::function<void(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&)> 
    fdDispatchFunction;

class udp_server : public sigslot::has_slots<>  
{
public:
    void start();

protected:
    void on_udp_msg(udp_socket*, ip_v4, USHORT, char*, int);

protected:
    virtual void register_dispatcher_()	;

protected:
    udp_socket socket_;
    dispatcher_mgr<UINT, fdDispatchFunction> dispatcher_;
};


}}