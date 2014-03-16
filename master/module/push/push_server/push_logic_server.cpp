#include "StdAfx.h"
#include "push_logic_server.h"

#include "../common/protocol_st.h"
#include "msg_send_mgr.h"

namespace logic
{


//----------------------------------------------------------------------
//
push_logic_server_module::push_logic_server_module(void)
{
}

push_logic_server_module::~push_logic_server_module(void)
{
}

bool push_logic_server_module::start(USHORT uport)
{
    auto mgr = msg_send_mgr::instance();
    mgr->sign_on_send.connect(this, &push_logic_server_module::send_msg);

    client_mgr_.sign_post_push_msg.connect(mgr, &msg_send_mgr::post_send_msg);
    return true;
}

void push_logic_server_module::send_msg(ip_v4 ip, USHORT port, push_msg_ptr msg)
{
    UINT uDelayTime = 0;
    PT_CMD_MESSAGE_CONTENT ptMsg;
    ptMsg.uMsgID            = msg->get_msg_id();
    ptMsg.uMsgType          = msg->get_msg_type();
    ptMsg.uShowTime         = msg->get_show_time();
    ptMsg.strMsgContent     = msg->get_content();
    ptMsg.strSign           = msg->get_str_sign();

    // 做一定的延时
    ptMsg.uDelayFetch = ++ uDelayTime % 10;
    ptMsg.uDelayShow  = ++ uDelayTime % 5;

    // 填充头部和消息体，并序列化到缓冲区中
    cmd_header header(CMD_MESSAGE_CONTENT);
    local_archive<> ar;
    ar << header;
    ar << ptMsg;

    socket_.send_to(ip, port, ar.get_buff(), ar.get_data_len());
}

void push_logic_server_module::on_query_curr_ver(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&)
{

}

void push_logic_server_module::on_register_online(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&)
{

}

void push_logic_server_module::on_message_content_ack(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&)
{

}


}