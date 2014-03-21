#include "stdafx.h"
#include "push_client_imp.h"

#include "../common/protocol.h"
#include "../common/protocol_st.h"

#include "app_data.h"
#include <net/proto/local_archive.h>


#pragma comment(lib, "sign_verify")

using namespace std::placeholders;


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
            pclient->reinit();// 判断是可能因为端口被限制，所以需要重新绑定端口
        }
    }
    else if (e->get_evt_type() == event_query_logic_ack_msg)
    {
        uNewStateID = status_query_newver; // 转换状态

        pclient->send_query_curver();
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
        pclient->send_query_curver(); // 发送查询最新版本消息

        // 判断是否超时触发了
        if (newver_watch_.is_triggerd()) 
        {
            newver_watch_.reset();

            // 一段时间内没有响应消息，则跳转到最初状态
            uNewStateID = status_query_logic_addr; 
            pclient->reinit();
        }
    }
    else if (e->get_evt_type() == event_query_ver_ack_msg) 
    {
        // 切换到在线状态
        uNewStateID = status_online ;

        pclient->send_online();
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
    last_online_ack_time_ = uTimeNow;

    is_online_ = FALSE;
}

void online_state::on_event(FsmEvent* e, UINT& uNewStateID)
{
    push_client_imp* pclient = push_client_imp::instance();

    if (e->get_evt_type() == event_timer) 
    {
        // 如果已经在线了，则要每隔30秒的时间再发送
        if (is_online_) 
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
        if (uTimeNow - last_online_ack_time_ > DEFAULT_ONLINE_TIME_OUT) 
        {
            is_online_ = FALSE;
            uNewStateID = status_query_logic_addr;  // 切换到查询逻辑服务器状态
            WriteLog("切换到查询逻辑服务器地址状态!!!");
        }
    }
    else if (e->get_evt_type() == event_online_ack_msg)
    {
        // 更新状态
        is_online_ = TRUE;
        last_online_ack_time_ = _time64(NULL);
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
    client_.set_callback(std::bind(&push_client_imp::on_msg,
        this, _1, _2, _3, _4, _5));

    push_fsm_.start();

    verify_helper_.set_pub_info("BA3F7EFF8A7AD3A6A445FD8C16E373F43C45A81D23E27588E13FCB8CA8A44F6E0E60AFD2E880759FF916A11025B1682A318FF0ADB3F45B5EE559E3D402A844B5DBEF71273D21AB8CE46F775964CE80617C645DF0994274041D993257A7F90BD50F0CDC84EA6AA31C766B2EB9A9832C0D13EE59EFF745B638AA0CCBAE6A939835", 
        "010001");
}

void push_client_imp::stop()
{
}

push_client_status push_client_imp::get_status()
{
    return (push_client_status) push_fsm_.get_cur_state_id();
}

void push_client_imp::reinit()
{
    client_.reinit();
}

void push_client_imp::on_msg(udp_client* client_, ip_v4 addr, USHORT uPort, char* buff, int iLen) 		///< UDP消息回调接口
{
    net_archive ar(buff, iLen);
    cmd_header header;
    ar >> header;

    if (header.cmd == CMD_QUERY_LOGIC_SERVER_ACK) 
    {
        on_query_logic_svr_ack(addr, uPort, header, ar);
    }
    else if (header.cmd == CMD_QUERY_CURRENT_VERSION_ACK) 
    {
        on_cur_ver_ack(addr, uPort, header, ar);
    }
    else if (header.cmd == CMD_MESSAGE_NOTIFY) 
    {
        on_msg_notify(addr, uPort, header, ar);
    }
    else if (header.cmd == CMD_MESSAGE_CONTENT) 
    {
        on_msg_content(addr, uPort, header, ar);
    }
    else if (header.cmd == CMD_ONLINE_ACK) 
    {
        on_online_msg_ack(addr, uPort, header, ar);
    }

    auto& data_callback = app_data::instance()->get_data_callback();
    if (data_callback)
        data_callback(client_, addr, uPort, buff, iLen);
}

void push_client_imp::send_query_logic_addr()
{
    WriteLog("发送查询逻辑服务器：%d消息...", app_data::instance()->get_svr_port());

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
        //@todo 向特定的ip发送数据
        //client_.send_to(BALANCE_SERVER_IP, BALANCE_SERVER_UDP_PORT, ar.GetBuff(), ar.GetDataLength());
    }
}

void push_client_imp::send_query_curver()
{
    WriteLog("发送查询最新版本消息");

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
    WriteLog("发送在线消息...");
    app_data* data = app_data::instance();

    // 定义并填充结构
    cmd_header header(CMD_ONLINE);
    PT_CMD_ONLINE cmd_online;

    cmd_online.mac = data->get_mac();
    cmd_online.channel = data->get_channel();
    cmd_online.last_msg_id = data->get_last_msg_id();
    cmd_online.login_name = data->get_user();
    cmd_online.version = CURRENT_CLIENT_VERSION_VALUE;
    cmd_online.account = data->get_user();

    // 序列化到缓冲区中
    local_archive<> ar;
    ar << header;
    ar << cmd_online;

    // 发送到逻辑服务器
    client_.send_to(data->get_logic_addr(), 
        data->get_logic_port(),
        ar.get_buff(), 
        ar.get_data_len());
}

void push_client_imp::send_msg_ack(UINT64 uMsgID)
{
    WriteLog("发送消息确认消息...");
    app_data* data = app_data::instance();

    // 定义并填充结构
    cmd_header header(CMD_MESSAGE_CONTENT_ACK);
    PT_CMD_MESSAGE_CONTENT_ACK cmd_msg_ack;

    cmd_msg_ack.uMsgID = uMsgID;
    cmd_msg_ack.emOpResult = OP_SUCCESS;

    local_archive<> ar;
    ar << header;
    ar << cmd_msg_ack;

    // 发送到逻辑服务器
    client_.send_to(data->get_logic_addr(), 
        data->get_logic_port(), 
        ar.get_buff(), 
        ar.get_data_len());
}

void push_client_imp::on_query_logic_svr_ack(ip_v4 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
{
    WriteLog("收到逻辑服务器查询回复消息...");
    app_data* data = app_data::instance();
    PT_CMD_QUERY_LOGIC_SERVER_ACK ack;
    ar >> ack;

    if (ar.get_error()) {
        WriteLog("响应查询逻辑服务器地址，协议出错!!!");
        return;
    }

    _ASSERT(push_fsm_.get_cur_state_id() == status_query_logic_addr);
    if (push_fsm_.get_cur_state_id() == status_query_logic_addr) 
    {
        _ASSERT(ack.uLogicIP > 0);
        _ASSERT(ack.uLogicUdpPort > 0);

        // 获得服务器的地址及端口
        data->set_logic_addr(ack.uLogicIP);
        data->set_logic_port(ack.uLogicUdpPort);

        // 状态机事件 
        message_event ev(event_query_logic_ack_msg);
        push_fsm_.on_event(&ev);

        WriteLog("逻辑服务器地址获取成功:%s:%d...", 
            inet_ntoa(*(in_addr*)& ack.uLogicIP), 
            ntohs(ack.uLogicUdpPort));
    }
}

void push_client_imp::on_online_msg_ack(ip_v4 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
{
    WriteLog("服务器回复在线消息...");

    if (uAddr == app_data::instance()->get_logic_addr()) 
    {
        PT_CMD_ONLINE_ACK ptOnlineAck;
        ar >> ptOnlineAck;
        if (ar.get_error()) {
            WriteLog("解析消息服务器在线消息出错...");
            return;
        }

        // 处理在线消息
        message_event ev(event_online_ack_msg);
        push_fsm_.on_event(&ev);
        return;
    }

    WriteLog("不是来自逻辑服务器的消息");
}

void push_client_imp::on_msg_notify(ip_v4 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
{
    WriteLog("服务器消息通知...");
    _ASSERT(FALSE && _T("暂未实现!!!"));

    PT_CMD_MESSAGE_NOTIFY ptNotify;

    ar  >> ptNotify;
    if (ar.get_error()) {
        WriteLog("解析消息通知出错");
        return;
    }

    app_data* data = app_data::instance();
    if (ptNotify.msgID <= data->get_last_msg_id()){
        return;
    }
}

void push_client_imp::on_msg_content(ip_v4 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
{
    WriteLog("服务器消息内容推送...");
    PT_CMD_MESSAGE_CONTENT ptMsg;
    ar >> ptMsg;

    app_data* data_ = app_data::instance();

    if (ar.get_error()) {
        WriteLog("解析消息内容出错...");
        return;
    }

    if (uAddr != data_->get_logic_addr()) {
        WriteLog("不是来自逻辑服务器的消息...");
        return;
    }

    BOOL bSignResult = verify_helper_.verify_sign(ptMsg.strSign, ptMsg.strMsgContent);
    if (bSignResult) 
    {
        if (data_->get_last_msg_id() == ptMsg.uMsgID) {
            WriteLog("pClientData->get_last_msg_id() == ptMsg.uMsgID 已接收过该消息");
            send_msg_ack(ptMsg.uMsgID);
            return;
        }

        // 发送反馈消息
        send_msg_ack(ptMsg.uMsgID);

        // 更新最新的消息ID
        data_->set_last_msg_id(ptMsg.uMsgID);
        data_->save();

        // 申请消息对象
        push_msg_ptr msg_(new push_msg);
        if (nullptr == msg_) {
            WriteLog("申请消息内存空间失败!!!");
            return;
        }

        // 验证通过后保存到消息列表中
        msg_->set_msg_id(ptMsg.uMsgID);
        msg_->set_msg_type(ptMsg.uMsgType);
        msg_->set_show_time(ptMsg.uShowTime);
        msg_->set_str_sign(std::move(ptMsg.strSign));
        msg_->set_content(std::move(ptMsg.strMsgContent));
        msg_->set_delay_fetch(ptMsg.uDelayFetch);
        msg_->set_delay_show(ptMsg.uDelayShow);
     
        // 提交到上层处理
        auto& callback = data_->get_msg_callback();
        if (callback)
            callback(msg_);

        return;
    }

    WriteLog("收到的消息验证不成功，判断不是来自服务器");
}

void push_client_imp::on_cur_ver_ack(ip_v4 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
{
    PT_CMD_QUERY_CURRENT_VERSION_ACK ptCurVersionAck;
    app_data* data_ = app_data::instance();

    // 读取消息内容
    ar >> ptCurVersionAck;

    // 非自动更新，且不是强制更新的话那么不进行升级
//     if (!pClientData->GetIsAutoUpdate() && !ptCurVersionAck.bForceUpdate) 
//     {
//         return;
//     }

    // 需要进行更新
    if (ptCurVersionAck.uVersionValue > CURRENT_CLIENT_VERSION_VALUE) 
    {
        // 开始下载最新的版本，并执行升级操作
        WriteLog("发现更高级的版本,ptCurVersionAck.uVersionValue > CURRENT_CLIENT_VERSION_VALUE...");

        // 更新
    }

    WriteLog("状态转换，转为: state_query_new_ver_ok");

    // 发送状态事件
    message_event ev(event_query_ver_ack_msg);
    push_fsm_.on_event(&ev);
}

bool push_client_imp::timer_check_status() 
{
    timer_event ev(3000);
    push_fsm_.on_event(&ev);

    return TRUE;
}