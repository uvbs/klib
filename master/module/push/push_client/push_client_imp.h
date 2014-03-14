#ifndef _push_client_imp_h_
#define _push_client_imp_h_


#include "../../include/push_interface.h"
#include <core/timer_mgr.h>
#include <comp/sign_verify/verify_helper.h>

///< 事件类型
enum em_event_type
{
    event_timer,                    ///< 定时器事件
    event_query_logic_ack_msg,      ///< 查询逻辑地址
    event_query_ver_ack_msg,        ///< 查询版本
    event_online_ack_msg,           ///< 在线消息回馈事件   
};


#define  DEFAULT_WATCH_TIMEOUT      (30)

///< 定时器事件结构定义
class timer_event : public FsmEvent
{
public:
    timer_event(UINT uElapse) : FsmEvent(event_timer)
    {
        set_evt_data(uElapse);
    }
};

///< 消息事件定义
class message_event : public FsmEvent
{
public: 
    message_event(UINT uMsg) : FsmEvent(uMsg)
    {
    }
};

///< 初始状态
class query_logic_state : public CState
{
public:
    query_logic_state() : CState(status_query_logic_addr), 
        query_addr_watch_(DEFAULT_WATCH_TIMEOUT) {}

public:
    virtual void enter(state_i* s);
    virtual void on_event(FsmEvent* e, UINT& uNewStateID);

protected:
    stop_watch query_addr_watch_;      ///< 在线处理
};

///< 查询新版本状态
class query_newver_state : public CState
{
public:
    query_newver_state() : 
        CState(status_query_newver), 
        newver_watch_(DEFAULT_WATCH_TIMEOUT)//, 
        //m_bDownloading(FALSE) 
    {}

public: 
    virtual void enter(state_i* s);
    virtual void on_event(FsmEvent* e, UINT& uNewStateID);

protected:
    //BOOL  m_bDownloading;
    stop_watch  newver_watch_;
};

///< 在线状态
class online_state : public CState
{
public:
    online_state() : CState(status_online) , online_watch_(DEFAULT_WATCH_TIMEOUT)
    {
        is_online_ = FALSE;
    }

    ///< 判断超时的时间值，如果超过这个值没有服务器的回应消息那么就会，转到查询逻辑服务器状态
    #define  DEFAULT_ONLINE_TIME_OUT  (2 * 60)          

public:
    ///< 需要初始化变量
    virtual void enter(state_i* s);

    ///< 处理定时器事件，及消息事件
    virtual void on_event(FsmEvent* e, UINT& uNewStateID);

protected:
    BOOL is_online_;                   ///< 指示是否在线成功
    UINT64  last_online_ack_time_;      ///< 上次的在线时间
    stop_watch online_watch_;           ///< 在线处理
};

///< 状态机的定义
BEGIN_FSM(push_client_fsm)
    BEGIN_REGISTER_STATE()
        REGISTER_INIT_STATE(query_logic_state)
        REGISTER_STATE(query_newver_state)
        REGISTER_STATE(online_state)
    END_REGISTER_STATE
END

//----------------------------------------------------------------------
// 客户端的具体实现类
class push_client_imp : 
    public singleton<push_client_imp>
{
    DECLARE_SINGLETON_CLASS(push_client_imp);

public:
    void start();
    void stop();
    push_client_status get_status();
    void reinit();

    DEFINE_ACCESS_FUN_REF2(udp_client, udp_client, client_);

protected:
    virtual void on_msg(udp_client* client_, UINT32 uAddr, USHORT uPort, char* buff, int iLen) ;		///< UDP消息回调接口

public:
    void send_query_logic_addr();
    void send_query_curver();                           ///< 发送查询当前版本消息
    void send_online();                                 ///< 发送在线消息 
    void send_offline();                                ///< 发送离线消息 
    void send_msg_ack(UINT64 uMsgID);                   ///< 发送ack消息

protected:
    // 处理网络来的消息 
    void on_query_logic_svr_ack(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar);
    void on_online_msg_ack(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar);///< CMD_ONLINE_ACK
    void on_msg_notify(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar);
    void on_msg_content(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar);
    void on_cur_ver_ack(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar);

protected:
    bool timer_check_status() ;           ///< 检查应用的状态

protected:
    udp_client      client_;
    push_client_fsm push_fsm_;
    timer_mgr       timer_mgr_;
    verify_helper   verify_helper_;
};


#endif // _push_client_imp_h_