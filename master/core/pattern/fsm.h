#pragma once

#ifdef _MSC_VER
#include <windows.h>
#endif
#include <set>
#include <map>
#include <stdio.h>
#include <iostream>

namespace klib
{
namespace pattern
{
namespace fsm
{

/*
状态机的实现

*/


///< 事件
class FsmEvent
{
public:
    FsmEvent() {}
    FsmEvent(UINT uEventType) { event_type_ = uEventType; }

    void set_evt_type(UINT utype) { event_type_ = utype; }
    UINT get_evt_type() { return event_type_; }

    void set_evt_data(UINT64 udata) {  event_data_ = udata; } 
    UINT64 get_evt_data() { return event_data_; }

protected:
    UINT    event_type_;         ///< 表示事件类型
    UINT64  event_data_;         ///< 携带事件数据
};

///< 状态接口
class state_i
{
public:
    virtual UINT get_state_id() = 0;                      ///< 返回状态的ID编号
    virtual void set_state_id(UINT uStateID) = 0;         ///< 设置状态的ID编号

    virtual char*  get_state_name() = 0;                  ///< 获取状态名称
    virtual void   set_state_name(char* pszStateName) = 0;   ///< 设置状态名称

    virtual void set_parent_id(UINT uParentID) = 0;       ///< 设置父状态
    virtual UINT get_parent_id() = 0;                     ///< 设置父状态

    virtual void enter(state_i* s) = 0;                  ///< 进入状态
    virtual void leave() = 0;                           ///< 离开状态
    virtual void on_event(FsmEvent* e, UINT& uNewStateID) = 0;                 ///< 处理事件
};

///< 状态基础类
class CState : public state_i
{
public:
    CState() : m_uParentID(0), m_uStateID(0) { m_szStateName[0] = 0; }
    CState(UINT uStateID) : m_uParentID(0), m_uStateID(uStateID) { m_szStateName[0] = 0; }
    virtual ~CState() {}

    virtual UINT get_state_id() {return m_uStateID;}
    virtual void set_state_id(UINT uStateID) { m_uStateID = uStateID; }

    virtual char*  get_state_name() { return m_szStateName; }
    virtual void   set_state_name(char* pszStateName) 
    { 
        strncpy_s(m_szStateName, _countof(m_szStateName) -1, pszStateName, strlen(pszStateName)); 
    }

    virtual void set_parent_id(UINT uParentID) { m_uParentID = uParentID; }
    virtual UINT get_parent_id() { return m_uParentID; }

    virtual void enter(state_i* s) {}
    virtual void leave() {}
    virtual void on_event(FsmEvent* e, UINT& uNewStateID) {}

private:
    UINT        m_uParentID;                ///< 父状态ID
    UINT        m_uStateID;                 ///< 当前状态ID
    char        m_szStateName[40];          ///< 当前状态名称
};

///< 状态机
class CFsm
{
public:
    CFsm() : cur_state_(NULL), exit_state_(NULL)  {}
    ~CFsm() 
    {
        auto itr = m_StateMap.begin();
        for (; itr  != m_StateMap.end(); ++ itr) 
        {
            delete itr->second;
        }
        m_StateMap.clear();
    }

public:
    ///< 获得当前的状态ID
    UINT get_cur_state_id() 
    {
        if (cur_state_) 
        {
            return cur_state_->get_state_id();
        }

        return ~0;
    }

    ///< 设置和获取初始状态
    void set_init_state(state_i* s)  { cur_state_ = s; }
    state_i* get_init_state() { return cur_state_; }

    ///< 设置和获取退出状态
    void set_exit_state(state_i* s) { exit_state_ = s; }
    state_i* get_exit_state(state_i* s) { return exit_state_; }

    ///< 添加状态
    void add_state(state_i* s) { m_StateMap.insert(std::make_pair(s->get_state_id(), s)); }

    ///< 删除相应的状态
    void del_state(UINT uStateID) 
    {
        state_i* s = get_state(uStateID);
        if (s) 
        {
            m_StateMap.erase(uStateID);
            delete s;
        }
    }

    ///< 根据状态ID获取状态
    state_i* get_state(UINT uStateID) 
    {
        auto itr = m_StateMap.find(uStateID);
        if (itr != m_StateMap.end()) 
        {
            return itr->second;
        }
        _ASSERT(FALSE);
        return NULL;
    }

    ///< 开始状态机
    void start()
    {
        if (cur_state_) 
        {
            cur_state_->enter(cur_state_);
        }
    }

    ///< 停止状态机
    void stop()
    {
        if (cur_state_ && cur_state_ == exit_state_) 
        {
            cur_state_->leave();
        }
    }

    ///< 改变状态
    void change_state(UINT uStateID)
    {
        if (cur_state_) 
        {
            if (uStateID != cur_state_->get_state_id()) 
            {
                state_i* pOldState = cur_state_;

                cur_state_->leave();
                cur_state_ = get_state(uStateID);
                cur_state_->enter(pOldState);
            }
        }
    }

public:
    void on_event(FsmEvent* e)
    {
        if (cur_state_) 
        {
            UINT uNewStateID = cur_state_->get_state_id();
            cur_state_->on_event(e, uNewStateID);

            if (uNewStateID != cur_state_->get_state_id()) 
            {
                state_i* pOldState = cur_state_;

                cur_state_->leave();
                cur_state_ = get_state(uNewStateID);
                cur_state_->enter(pOldState);
            }
        }
    }

protected:
    std::map<UINT, state_i*> m_StateMap;   ///< 状态表

    state_i* cur_state_;                  ///< 当前状态指针
    state_i* exit_state_;
};


//------------------------------------------------------------------------
// 使用宏定义，定义状态机
#define  BEGIN                                                  \
{

#define   END           };

#define  BEGIN_STATE_DECLARE(State_Name, State_ID)              \
class State_Name : public CState                                \
{                                                               \
    enum {STATE_ID = State_ID};                                 \
    public: State_Name() : CState(State_Name::STATE_ID)         \
    {                                                           \
        this->set_state_name(#State_Name);                        \
    }

///< 开始定义on_event
#define  BEGIN_ON_EVENT()                                       \
public: void on_event(Event* e, UINT& uNewStateID)               \
{

///< 结束定义on_event
#define  END_ON_EVENT                                           \
}

///< 开始定义leave
#define  BEGIN_LEAVE()                                          \
public: void leave()                                            \
{

///< 结束定义leave
#define  END_LEAVE                                              \
}

///< 开始定义enter
#define  BEGIN_ENTER()                                          \
public: void enter(state_i* s)                                   \
{

///< 结束定义enter
#define  END_ENTER                                              \
}

///< 开始定义fsm
#define  BEGIN_FSM(FSM_Name)                                    \
class FSM_Name : public CFsm                                    \
{                                                               \
public:                                                         \
    FSM_Name()                                                  \
    {                                                           \
        RegisterAllState();                                     \
    }

///< 开始注册状态
#define  BEGIN_REGISTER_STATE()                                 \
void RegisterAllState()                                         \
{                                                               \
    state_i* s = NULL;

///< 注册状态
#define  REGISTER_STATE(State_Name)                             \
    this->add_state(new State_Name);                             \

///< 注册初始状态
#define REGISTER_INIT_STATE(State_Name)                         \
    s = new State_Name;                                         \
    if (s) {                                                    \
        this->add_state(s);                                      \
        this->set_init_state(s);                                  \
    }

///< 注册结束状态
#define REGISTER_EXIT_STATE(State_Name)                         \
    s = new State_Name;                                         \
    this->add_state(s);                                          \
    this->set_exit_state(s);

///< 结束注册状态
#define  END_REGISTER_STATE                                     \
}

///< 结束fsm定义
#define  END_FSM                                                \
};


/*

enum status_test
{
LogicState,
NewVerState,
OnlineState
}

BEGIN_STATE_DECLARE(CQueryLogicState, LogicState)

virtual void on_event(FsmEvent* e, UINT& uNewStateID) {}
END

BEGIN_STATE_DECLARE(CQueryNewVerState, NewVerState)

virtual void on_event(FsmEvent* e, UINT& uNewStateID) {}
END

BEGIN_STATE_DECLARE(COnlineState, OnlineState)

virtual void on_event(FsmEvent* e, UINT& uNewStateID) {}
END


///< 状态机的定义
BEGIN_FSM(ClientAppFsm)
    BEGIN_REGISTER_STATE()
        REGISTER_INIT_STATE(CQueryLogicState)
        REGISTER_STATE(CQueryNewVerState)
        REGISTER_STATE(COnlineState)
    END_REGISTER_STATE
END

ClientAppFsm    m_fsm;
m_fsm.start();

*/


}

}}