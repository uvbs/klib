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
    mgr->s_send_msg.connect(this, &push_logic_server_module::send_msg);

    client_mgr_.s_on_post_push_msg.connect(mgr, &msg_send_mgr::post_send_msg);
    return true;
}

bool push_logic_server_module::post_send_msg(ip_v4 addr, USHORT port, push_msg_ptr msg)
{
    client_key k(addr, port);
    if (!client_mgr_.is_client_exists(k))
        return false;
    
    msg_send_mgr* mgr_ = msg_send_mgr::instance();
    mgr_->post_send_msg(addr, port, msg);

    return true;
}

size_t push_logic_server_module::get_online_client_count()
{
    return client_mgr_.get_online_client_count();
}

void push_logic_server_module::send_online_ack(ip_v4 ip, USHORT port)
{
    local_archive<> ar;
    cmd_header ackHeader;
    PT_CMD_ONLINE_ACK ptOnlineAck;
    
    ackHeader.cmd = CMD_ONLINE_ACK;
    ptOnlineAck.bStartMini = false;
    ptOnlineAck.msgId = 0;
    ptOnlineAck.uuid  = 0;
    ar << ackHeader;
    ar << ptOnlineAck;

    socket_.send_to(ip, port, ar.get_buff(), ar.get_data_len());
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

void push_logic_server_module::on_query_curr_ver(ip_v4 ip, 
    USHORT port, 
    cmd_header* header, 
    net_archive*, BOOL& )
{
    auto mgr = msg_send_mgr::instance();
    PT_CMD_QUERY_CURRENT_VERSION_ACK ptVersionAck;
    ptVersionAck.uVersionValue = MAKEWORD(2,2);//m_objCurrentVer.uVerValue;
    ptVersionAck.strUpdateUrl =  "http://";//m_objCurrentVer.strUpdateUrl;
    ptVersionAck.strVersion =  "2.2";//m_objCurrentVer.strVer;
    ptVersionAck.strUpdateSign = "";
    ptVersionAck.bClientRequest = false;

    cmd_header hdr(CMD_QUERY_CURRENT_VERSION_ACK);
    local_archive<> out;

    out << hdr;
    out << ptVersionAck;
    socket_.send_to(ip, port, out.get_buff(), out.get_data_len());
}

void push_logic_server_module::on_register_online(ip_v4 ip, 
    USHORT port, 
    cmd_header* header, 
    net_archive*, 
    BOOL&)
{
    client_key key(ip, port);
    BOOL is_new = FALSE;

    local_archive<> ar;
    PT_CMD_ONLINE ptOnline;
    ar >> ptOnline;
    if (ar.get_error()) 
        return;

    client_info* pInfo = client_mgr_.update_client_info(key, ptOnline, is_new);
    if (nullptr == pInfo) 
        return;

    if (is_new) 
        pInfo->set_last_msg_id(ptOnline.last_msg_id);

    MyPrtLog(("%s:%d send online message..."), inet_ntoa(*(in_addr*)&ip), ntohs(port));
    send_online_ack(ip, port);
}

void push_logic_server_module::on_message_content_ack(ip_v4 ip, 
    USHORT port, 
    cmd_header* header, 
    net_archive*, 
    BOOL&)
{
    PT_CMD_MESSAGE_CONTENT_ACK ptMsgAck;
    local_archive<> ar;
    ar >> ptMsgAck;
    if (ar.get_error()) 
        return;

    // 客户端处理成功。
    if (ptMsgAck.emOpResult == OP_SUCCESS) 
    {
        client_mgr_.add_client_confirm_msg_id(ip, port, ptMsgAck.uMsgID);

        auto mgr = msg_send_mgr::instance();
        mgr->remove_msg_confirm_info(ptMsgAck.uMsgID);
    }
    else 
    {
        MyPrtLog(_T("client confirm message error!!!"));
        _ASSERT(FALSE);
    }
}


}