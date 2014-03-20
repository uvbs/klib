#pragma once

#include "../../include/push_interface.h"
#include "../common/protocol.h"

#include <net/udp_server.h>
#include <net/proto/net_archive.h>
#include <net/proto/local_archive.h>

using namespace std::placeholders;

#include "client_mgr.h"

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
        INTERFACE_ENTRY(IID_PUSH_LOGIC_SERVER, push_logic_server_i)
    END_INTERFACE_ENTRY

public:
    BEGIN_REGISTER_DIAPATCH()
        REGISTER_DISPATCH(CMD_QUERY_CURRENT_VERSION, std::bind(&push_logic_server_module::on_query_curr_ver, this, _1, _2, _3, _4, _5))
        REGISTER_DISPATCH(CMD_ONLINE, std::bind(&push_logic_server_module::on_register_online, this, _1, _2, _3, _4, _5))
        REGISTER_DISPATCH(CMD_MESSAGE_CONTENT_ACK, std::bind(&push_logic_server_module::on_message_content_ack, this, _1, _2, _3, _4, _5))
    END_REGISTER_DISPATCH

    virtual void default_msg_handler(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&){}

public:
    //----------------------------------------------------------------------
    // push_logic_server_i
    void set_handle(
        handle_client_online_callback online_handle,
        handle_client_offline_callback offline_handle,
        handle_send_msg_result_callback msg_result_handle);
    bool start(USHORT uport);   ///< 启动服务
    bool post_send_msg(ip_v4 addr, USHORT port, push_msg_ptr msg); ///< 发送消息
    size_t get_online_client_count(); ///< 在线客户端个数

public:
    // 对其它方法提供的函数
    void send_msg(ip_v4 ip, USHORT port, push_msg_ptr msg);
    void send_online_ack(ip_v4 ip, USHORT port);

protected:
    // 处理客户端发来的消息
    void on_query_curr_ver(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&);
    void on_register_online(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&);
    void on_message_content_ack(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&);

protected:
    client_mgr client_mgr_;
};


}