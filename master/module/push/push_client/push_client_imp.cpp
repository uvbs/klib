#include "stdafx.h"
#include "push_client_imp.h"

#include "../common/protocol.h"
#include "../common/protocol_st.h"

#include "app_data.h"
#include <net/proto/local_archive.h>


#pragma comment(lib, "sign_verify")

using namespace std::placeholders;

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
            pclient->reinit();
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
            pclient->reinit();
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
            MyPrtLog("切换到查询逻辑服务器地址状态!!!");
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

void push_client_imp::on_msg(udp_client* client_, UINT32 uAddr, USHORT uPort, char* buff, int iLen) 		///< UDP消息回调接口
{
    MyPrtLog("message comming!!!");

    net_archive ar(buff, iLen);

    cmd_header header;
    ar >> header;

    if (header.cmd == CMD_QUERY_LOGIC_SERVER_ACK) 
    {
        on_query_logic_svr_ack(uAddr, uPort, header, ar);
    }
    else if (header.cmd == CMD_QUERY_CURRENT_VERSION_ACK) 
    {
        on_cur_ver_ack(uAddr, uPort, header, ar);
    }
    else if (header.cmd == CMD_MESSAGE_NOTIFY) 
    {
        on_msg_notify(uAddr, uPort, header, ar);
    }
    else if (header.cmd == CMD_MESSAGE_CONTENT) 
    {
        on_msg_content(uAddr, uPort, header, ar);
    }
    else if (header.cmd == CMD_ONLINE_ACK) 
    {
        on_online_msg_ack(uAddr, uPort, header, ar);
    }
}

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
        //@todo 向特定的ip发送数据
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
    ptOnline.mac = data->get_mac();
    ptOnline.channel = data->get_channel();
    ptOnline.last_msg_id = data->get_last_msg_id();
    ptOnline.login_name = data->get_user();
    ptOnline.version = CURRENT_CLIENT_VERSION_VALUE;
    ptOnline.account = data->get_user();

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

void push_client_imp::on_query_logic_svr_ack(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
{
    MyPrtLog("收到逻辑服务器查询回复消息...");

    PT_CMD_QUERY_LOGIC_SERVER_ACK ack;
    ar >> ack;

    if (ar.get_error()) {
        MyPrtLog(_T("响应查询逻辑服务器地址，协议出错!!!"));
        return;
    }

    _ASSERT(push_fsm_.get_cur_state_id() == status_query_logic_addr);
    if (push_fsm_.get_cur_state_id() == status_query_logic_addr) 
    {
        // 获得服务器的地址及端口
        app_data::instance()->set_logic_addr(ack.uLogicIP);
        app_data::instance()->set_logic_port(ack.uLogicUdpPort);

        _ASSERT(ack.uLogicIP > 0);
        _ASSERT(ack.uLogicUdpPort > 0);

        // 状态机事件 
        message_event ev(event_query_logic_ack_msg);
        push_fsm_.on_event(&ev);

        MyPrtLog("逻辑服务器地址获取成功:%s:%d...", 
            inet_ntoa(*(in_addr*)& ack.uLogicIP), 
            ntohs(ack.uLogicUdpPort));
    }
}

void push_client_imp::on_online_msg_ack(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
{
    MyPrtLog(_T("服务器回复在线消息..."));

    if (uAddr == app_data::instance()->get_logic_addr()) 
    {
        PT_CMD_ONLINE_ACK ptOnlineAck;
        ar >> ptOnlineAck;
        if (ar.get_error()) {
            MyPrtLog(_T("解析消息服务器在线消息出错..."));
            return;
        }

        // 处理在线消息
        message_event ev(event_online_ack_msg);
        push_fsm_.on_event(&ev);
        return;
    }

    MyPrtLog(_T("不是来自逻辑服务器的消息"));
}

void push_client_imp::on_msg_notify(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
{
    MyPrtLog(_T("服务器消息通知..."));
    _ASSERT(FALSE && _T("暂未实现!!!"));

    PT_CMD_MESSAGE_NOTIFY ptNotify;

    ar  >> ptNotify;
    if (ar.get_error()) {
        MyPrtLog(_T("解析消息通知出错"));
        return;
    }

    app_data* data = app_data::instance();
    if (ptNotify.msgID <= data->get_last_msg_id()){
        return;
    }
}

void push_client_imp::on_msg_content(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
{
    MyPrtLog(_T("服务器消息内容推送..."));
    PT_CMD_MESSAGE_CONTENT ptMsg;
    ar >> ptMsg;

    app_data* data_ = app_data::instance();

    if (ar.get_error()) {
        MyPrtLog(_T("解析消息内容出错..."));
        return;
    }

    if (uAddr != data_->get_logic_addr()) {
        MyPrtLog(_T("不是来自逻辑服务器的消息..."));
        return;
    }

    BOOL bSignResult = verify_helper_.verify_sign(ptMsg.strSign, ptMsg.strMsgContent);
    if (bSignResult) 
    {
        if (data_->get_last_msg_id() == ptMsg.uMsgID) {
            MyPrtLog(_T("pClientData->m_uLastMsgId == ptMsg.uMsgID 已接收过该消息"));
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
            MyPrtLog(_T("申请消息内存空间失败!!!"));
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
        callback(msg_);
        return;
    }

    MyPrtLog(_T("收到的消息验证不成功，判断不是来自服务器"));
}

void push_client_imp::on_cur_ver_ack(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
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
        MyPrtLog(_T("发现更高级的版本,ptCurVersionAck.uVersionValue > CURRENT_CLIENT_VERSION_VALUE..."));

        // 更新
    }

    MyPrtLog(_T("状态转换，转为: state_query_new_ver_ok"));

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