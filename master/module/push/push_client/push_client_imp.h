#ifndef _push_client_imp_h_
#define _push_client_imp_h_


#include "../../include/push_interface.h"


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
class CTimerEvent : public FsmEvent
{
public:
    CTimerEvent(UINT uElapse) : FsmEvent(event_timer)
    {
        set_evt_data(uElapse);
    }
};

///< 消息事件定义
class CMessageEvent : public FsmEvent
{
public: 
    CMessageEvent(UINT uMsg) : FsmEvent(uMsg)
    {
    }
};

///< 初始状态
class CQueryLogicState : public CState
{
public:
    CQueryLogicState() : CState(status_query_logic_addr), 
        query_addr_watch_(DEFAULT_WATCH_TIMEOUT) {}

public:
    virtual void Enter(IState* s);
    virtual void OnEvent(FsmEvent* e, UINT& uNewStateID);

protected:
    stop_watch query_addr_watch_;      ///< 在线处理
};

///< 查询新版本状态
class CQueryNewVerState : public CState
{
public:
    CQueryNewVerState() : 
        CState(status_query_newver), 
        newver_watch_(DEFAULT_WATCH_TIMEOUT)//, 
        //m_bDownloading(FALSE) 
    {}

public: 
    virtual void Enter(IState* s);
    virtual void OnEvent(FsmEvent* e, UINT& uNewStateID);

protected:
    //BOOL  m_bDownloading;
    stop_watch  newver_watch_;
};

///< 在线状态
class COnlineState : public CState
{
public:
    COnlineState() : CState(status_online) , online_watch_(DEFAULT_WATCH_TIMEOUT)
    {
        m_bOnlined = FALSE;
    }

    ///< 判断超时的时间值，如果超过这个值没有服务器的回应消息那么就会，转到查询逻辑服务器状态
    #define  DEFAULT_ONLINE_TIME_OUT  (2 * 60)          

public:
    ///< 需要初始化变量
    virtual void Enter(IState* s);

    ///< 处理定时器事件，及消息事件
    virtual void OnEvent(FsmEvent* e, UINT& uNewStateID);

protected:
    BOOL m_bOnlined;                   ///< 指示是否在线成功
    UINT64  m_uLastOnlineAckTime;      ///< 上次的在线时间
    stop_watch online_watch_;      ///< 在线处理
};

///< 状态机的定义
BEGIN_FSM(push_client_fsm)
    BEGIN_REGISTER_STATE()
        REGISTER_INIT_STATE(CQueryLogicState)
        REGISTER_STATE(CQueryNewVerState)
        REGISTER_STATE(COnlineState)
    END_REGISTER_STATE
END

//----------------------------------------------------------------------
// 客户端的具体实现类
class push_client_imp : public singleton<push_client_imp>
{
    DECLARE_SINGLETON_CLASS(push_client_imp);
public:

    void start();
    void stop();
    push_client_status get_status();

    DEFINE_ACCESS_FUN_REF2(udp_client, udp_client, client_);
    DEFINE_ACCESS_FUN2(bool, updating, updating_);

public:
    void SendQueryLogicAddr();
    void SendQueryCurVer();                          ///< 发送查询当前版本消息
    void SendOnline();                               ///< 发送在线消息 
    void SendOffline();                              ///< 发送离线消息 
    void SendMessageAck(UINT64 uMsgID);              ///< 发送ack消息

protected:
    udp_client client_;
    push_client_fsm push_fsm_;

    bool updating_;  // @todo
};


#endif // _push_client_imp_h_