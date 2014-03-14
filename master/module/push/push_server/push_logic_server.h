#pragma once

#include "../../include/push_interface.h"

#include "../common/protocol.h"
#include <net/udp_server.h>
#include <net/proto/net_archive.h>
#include <net/proto/local_archive.h>

using namespace std::placeholders;


namespace logic
{

//----------------------------------------------------------------------
// 逻辑服务器
class push_logic_server_module : 
    public module_imp,
    public udp_server,
    public push_logic_server_i
{
public:
    push_logic_server_module(void);
    ~push_logic_server_module(void);

public:
    BEGIN_INTERFACE_ENTRY
        INTERFACE_ENTRY(IID_PUSH_BALANCE_SERVER, push_logic_server_i)
    END_INTERFACE_ENTRY

public:
    BEGIN_REGISTER_DIAPATCH()
        REGISTER_DISPATCH(CMD_QUERY_CURRENT_VERSION, std::bind(&push_logic_server_module::on_query_curr_ver, this, _1, _2, _3, _4, _5))
        REGISTER_DISPATCH(CMD_ONLINE, std::bind(&push_logic_server_module::on_register_online, this, _1, _2, _3, _4, _5))
        REGISTER_DISPATCH(CMD_MESSAGE_CONTENT_ACK, std::bind(&push_logic_server_module::on_message_content_ack, this, _1, _2, _3, _4, _5))
    END_REGISTER_DISPATCH

public:
    // 启动服务
    bool start(USHORT uport);

    void send_msg(ip_v4 ip, USHORT port, push_msg_ptr msg);

protected:
    void on_query_curr_ver(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&);
    void on_register_online(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&);
    void on_message_content_ack(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&);
};


}