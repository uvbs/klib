#ifndef _push_client_imp_h_
#define _push_client_imp_h_


#include "../../include/push_interface.h"


///< �¼�����
enum em_event_type
{
    event_timer,                    ///< ��ʱ���¼�
    event_query_logic_ack_msg,      ///< ��ѯ�߼���ַ
    event_query_ver_ack_msg,        ///< ��ѯ�汾
    event_online_ack_msg,           ///< ������Ϣ�����¼�   
};


#define  DEFAULT_WATCH_TIMEOUT      (30)

///< ��ʱ���¼��ṹ����
class CTimerEvent : public FsmEvent
{
public:
    CTimerEvent(UINT uElapse) : FsmEvent(event_timer)
    {
        set_evt_data(uElapse);
    }
};

///< ��Ϣ�¼�����
class CMessageEvent : public FsmEvent
{
public: 
    CMessageEvent(UINT uMsg) : FsmEvent(uMsg)
    {
    }
};

///< ��ʼ״̬
class CQueryLogicState : public CState
{
public:
    CQueryLogicState() : CState(status_query_logic_addr), 
        query_addr_watch_(DEFAULT_WATCH_TIMEOUT) {}

public:
    virtual void Enter(IState* s);
    virtual void OnEvent(FsmEvent* e, UINT& uNewStateID);

protected:
    stop_watch query_addr_watch_;      ///< ���ߴ���
};

///< ��ѯ�°汾״̬
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

///< ����״̬
class COnlineState : public CState
{
public:
    COnlineState() : CState(status_online) , online_watch_(DEFAULT_WATCH_TIMEOUT)
    {
        m_bOnlined = FALSE;
    }

    ///< �жϳ�ʱ��ʱ��ֵ������������ֵû�з������Ļ�Ӧ��Ϣ��ô�ͻᣬת����ѯ�߼�������״̬
    #define  DEFAULT_ONLINE_TIME_OUT  (2 * 60)          

public:
    ///< ��Ҫ��ʼ������
    virtual void Enter(IState* s);

    ///< ������ʱ���¼�������Ϣ�¼�
    virtual void OnEvent(FsmEvent* e, UINT& uNewStateID);

protected:
    BOOL m_bOnlined;                   ///< ָʾ�Ƿ����߳ɹ�
    UINT64  m_uLastOnlineAckTime;      ///< �ϴε�����ʱ��
    stop_watch online_watch_;      ///< ���ߴ���
};

///< ״̬���Ķ���
BEGIN_FSM(push_client_fsm)
    BEGIN_REGISTER_STATE()
        REGISTER_INIT_STATE(CQueryLogicState)
        REGISTER_STATE(CQueryNewVerState)
        REGISTER_STATE(COnlineState)
    END_REGISTER_STATE
END

//----------------------------------------------------------------------
// �ͻ��˵ľ���ʵ����
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
    void SendQueryCurVer();                          ///< ���Ͳ�ѯ��ǰ�汾��Ϣ
    void SendOnline();                               ///< ����������Ϣ 
    void SendOffline();                              ///< ����������Ϣ 
    void SendMessageAck(UINT64 uMsgID);              ///< ����ack��Ϣ

protected:
    udp_client client_;
    push_client_fsm push_fsm_;

    bool updating_;  // @todo
};


#endif // _push_client_imp_h_