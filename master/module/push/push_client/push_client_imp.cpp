#include "stdafx.h"
#include "push_client_imp.h"

#include "../common/protocol.h"
#include "../common/protocol_st.h"

#include "app_data.h"
#include <net/proto/local_archive.h>

//----------------------------------------------------------------------
//

//----------------------------------------------------------------------
// 状态相关
void query_logic_state::enter(state_i* s)
{
    query_addr_watch_.reset();
}

void query_logic_state::on_event(FsmEvent* e, UINT& uNewStateID)
{
    push_client_imp* pclient = push_client_imp::instance();

    if (e->get_evt_type() == event_timer) 
    {
        pclient->send_query_logic_addr();

        if (query_addr_watch_.is_triggerd()) 
        {
            query_addr_watch_.reset();

            // 判断是可能因为端口被限制，所以需要重新绑定端口
            pclient->get_udp_client().reinit();
        }
    }
    else if (e->get_evt_type() == event_query_logic_ack_msg)
    {
        // 转换状态
        uNewStateID = status_query_newver;
    }
    else
    {
        _ASSERT(FALSE);
    }
}

void query_newver_state::enter(state_i* s)
{
    // 需要重置定时监视器
    newver_watch_.reset();
}

void query_newver_state::on_event(FsmEvent* e, UINT& uNewStateID)
{
    push_client_imp* pclient = push_client_imp::instance();

    if (e->get_evt_type() == event_timer) 
    {
        // 发送查询最新版本消息
        pclient->send_query_curver();

        // 判断是否超时触发了
        if (newver_watch_.is_triggerd()) 
        {
            newver_watch_.reset();

            uNewStateID = status_query_logic_addr;

            // 判断是可能因为端口被限制，所以需要重新绑定端口
            pclient->get_udp_client().reinit();
        }
    }
    else if (e->get_evt_type() == event_query_ver_ack_msg) 
    {
        // 切换到在线状态
        uNewStateID = status_online ;
    }
    else
    {
        _ASSERT(FALSE);
    }
}

void online_state::enter(state_i* s)
{
    UINT64 uTimeNow = _time64(NULL);

    // 假设收到了在线的回馈消息
    m_uLastOnlineAckTime = uTimeNow;

    m_bOnlined = FALSE;
}

void online_state::on_event(FsmEvent* e, UINT& uNewStateID)
{
    push_client_imp* pclient = push_client_imp::instance();

    if (e->get_evt_type() == event_timer) 
    {
        // 如果已经在线了，则要每隔30秒的时间再发送
        if (m_bOnlined) 
        {
            // 大于30秒的时候发送一次心跳记录
            if (online_watch_.is_triggerd()) 
            {
                online_watch_.reset();
                pclient->send_online();
            }
        }
        else
        {
            // 按规律发送
            online_watch_.reset();
            pclient->send_online();
        }

        // 处理长时间未收到在线消息回馈的情况，需要重新获取逻辑地址
        UINT64 uTimeNow = _time64(NULL);
        if (uTimeNow - m_uLastOnlineAckTime > DEFAULT_ONLINE_TIME_OUT) 
        {
            m_bOnlined = FALSE;
            uNewStateID = status_query_logic_addr;  // 切换到查询逻辑服务器状态
            MyPrtLog("切换到查询逻辑服务器地址状态!!!");
        }
    }
    else if (e->get_evt_type() == event_online_ack_msg)
    {
        // 更新状态
        m_bOnlined = TRUE;
        m_uLastOnlineAckTime = _time64(NULL);
    }
    else
    {
        _ASSERT(FALSE);
    }
}

//----------------------------------------------------------------------
//
void push_client_imp::start()
{
    timer_mgr_.start();
    timer_mgr_.add(3000, std::bind(&push_client_imp::timer_check_status, this));

    client_.start(FALSE);
    client_.enable_udpreset(TRUE);
    client_.set_handler(this);
}

void push_client_imp::stop()
{
}

push_client_status push_client_imp::get_status()
{
    return (push_client_status) push_fsm_.get_cur_statei_d();
}

void push_client_imp::on_msg(udp_client* client_, UINT32 uAddr, USHORT uPort, char* buff, int iLen) 		///< UDP消息回调接口
{
    MyPrtLog("message comming!!!");
}

//
void push_client_imp::send_query_logic_addr()
{
    MyPrtLog("发送查询逻辑服务器：%d消息...", app_data::instance()->get_svr_port());

    cmd_header header;
    PT_CMD_QUERY_LOGIC_SERVER ptQuery;

    header.cmd = CMD_QUERY_LOGIC_SERVER;
    ptQuery.uuid = 123456; // @todo 这里需要重新定义

    local_archive<> ar;
    ar << header;
    ar << ptQuery;

    BOOL bSendResult = client_.send_to(app_data::instance()->get_svr_domain().c_str(), 
        app_data::instance()->get_svr_port(), 
        ar.get_buff(), 
        ar.get_data_len());

    if (!bSendResult) {
        //@todo 
        //client_.send_to(BALANCE_SERVER_IP, BALANCE_SERVER_UDP_PORT, ar.GetBuff(), ar.GetDataLength());
    }
}

void push_client_imp::send_query_curver()
{
    MyPrtLog(_T("发送查询最新版本消息"));

    cmd_header header(CMD_QUERY_CURRENT_VERSION);
    PT_CMD_QUERY_CURRENT_VERSION ptQueryCurVer;
    ptQueryCurVer.strVersion   = CURRENT_CLIENT_VERSION_STRING;
    ptQueryCurVer.uCurVerValue = CURRENT_CLIENT_VERSION_VALUE;

    local_archive<> ar;
    ar << header;
    ar << ptQueryCurVer;

    client_.send_to(app_data::instance()->get_logic_addr(), 
        app_data::instance()->get_logic_port(), 
        ar.get_buff(), 
        ar.get_data_len());
}

void push_client_imp::send_online()
{
    MyPrtLog("发送在线消息...");
    app_data* data = app_data::instance();

    // 定义并填充结构
    cmd_header header;
    PT_CMD_ONLINE ptOnline;

    header.cmd = CMD_ONLINE;
    ptOnline.strMac = data->get_mac();
    ptOnline.strChannel = data->get_channel();
    ptOnline.uLastMsgID = data->get_last_msg_id();
    ptOnline.strLoginName = data->get_user();
    ptOnline.uVersion = CURRENT_CLIENT_VERSION_VALUE;
    ptOnline.strAccount = data->get_user();

    // 序列化到缓冲区中
    local_archive<> ar;
    ar << header;
    ar << ptOnline;

    // 发送到逻辑服务器
    client_.send_to(app_data::instance()->get_logic_addr(), 
        app_data::instance()->get_logic_port(),
        ar.get_buff(), 
        ar.get_data_len());
}

void push_client_imp::send_msg_ack(UINT64 uMsgID)
{
    MyPrtLog("发送消息确认消息...");

    // 定义并填充结构
    cmd_header header;
    PT_CMD_MESSAGE_CONTENT_ACK ptMsgAck;

    header.cmd = CMD_MESSAGE_CONTENT_ACK;
    ptMsgAck.uMsgID = uMsgID;
    ptMsgAck.emOpResult = OP_SUCCESS;

    local_archive<> ar;
    ar << header;
    ar << ptMsgAck;

    // 发送到逻辑服务器
    client_.send_to(app_data::instance()->get_logic_addr(), 
        app_data::instance()->get_logic_port(), 
        ar.get_buff(), 
        ar.get_data_len());
}

bool push_client_imp::timer_check_status() 
{
    timer_event ev(3000);
    push_fsm_.on_event(&ev);

    return TRUE;
}