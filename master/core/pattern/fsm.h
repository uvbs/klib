#pragma once


#include <windows.h>
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
    FsmEvent(UINT uEventType) { m_uEventType = uEventType; }

    UINT    m_uEventType;         ///< 表示事件类型
    UINT64  m_uEventData;         ///< 携带事件数据
};

///< 状态接口
class IState
{
public:
    virtual UINT GetStateID() = 0;                      ///< 返回状态的ID编号
    virtual void SetStateID(UINT uStateID) = 0;         ///< 设置状态的ID编号

    virtual char*  GetStateName() = 0;                  ///< 获取状态名称
    virtual void   SetStateName(char* pszStateName) = 0;   ///< 设置状态名称

    virtual void SetParentID(UINT uParentID) = 0;       ///< 设置父状态
    virtual UINT GetParentID() = 0;                     ///< 设置父状态

    virtual void Enter(IState* s) = 0;                  ///< 进入状态
    virtual void Leave() = 0;                           ///< 离开状态
    virtual void OnEvent(FsmEvent* e, UINT& uNewStateID) = 0;                 ///< 处理事件
};

///< 状态基础类
class CState : public IState
{
public:
    CState() : m_uParentID(0), m_uStateID(0) { m_szStateName[0] = 0; }
    CState(UINT uStateID) : m_uParentID(0), m_uStateID(uStateID) { m_szStateName[0] = 0; }
    virtual ~CState() {}

    virtual UINT GetStateID() {return m_uStateID;}
    virtual void SetStateID(UINT uStateID) { m_uStateID = uStateID; }

    virtual char*  GetStateName() { return m_szStateName; }
    virtual void   SetStateName(char* pszStateName) 
    { 
        strncpy_s(m_szStateName, _countof(m_szStateName) -1, pszStateName, strlen(pszStateName)); 
    }

    virtual void SetParentID(UINT uParentID) { m_uParentID = uParentID; }
    virtual UINT GetParentID() { return m_uStateID; }

    virtual void Enter(IState* s) {}
    virtual void Leave() {}
    virtual void OnEvent(FsmEvent* e, UINT& uNewStateID) {}

private:
    UINT        m_uParentID;                ///< 父状态ID
    UINT        m_uStateID;                 ///< 当前状态ID
    char        m_szStateName[20];          ///< 当前状态名称
};

///< 状态机
class CFsm
{
public:
    CFsm() : m_pCurState(NULL), m_pExitState(NULL)  {}
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
    UINT GetCurStateID() 
    {
        if (m_pCurState) 
        {
            return m_pCurState->GetStateID();
        }

        return ~0;
    }

    ///< 设置和获取初始状态
    void SetInitState(IState* s)  { m_pCurState = s; }
    IState* GetInitState() { return m_pCurState; }

    ///< 设置和获取退出状态
    void SetExitState(IState* s) { m_pExitState = s; }
    IState* GetExitState(IState* s) { return m_pExitState; }

    ///< 添加状态
    void AddState(IState* s) { m_StateMap.insert(std::make_pair(s->GetStateID(), s)); }

    ///< 删除相应的状态
    void DelState(UINT uStateID) 
    {
        IState* s = GetState(uStateID);
        if (s) 
        {
            m_StateMap.erase(uStateID);
            delete s;
        }
    }

    ///< 根据状态ID获取状态
    IState* GetState(UINT uStateID) 
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
    void Start()
    {
        if (m_pCurState) 
        {
            m_pCurState->Enter(m_pCurState);
        }
    }

    ///< 停止状态机
    void Stop()
    {
        if (m_pCurState && m_pCurState == m_pExitState) 
        {
            m_pCurState->Leave();
        }
    }

    ///< 改变状态
    void ChangeState(UINT uStateID)
    {
        if (m_pCurState) 
        {
            if (uStateID != m_pCurState->GetStateID()) 
            {
                IState* pOldState = m_pCurState;

                m_pCurState->Leave();
                m_pCurState = GetState(uStateID);
                m_pCurState->Enter(pOldState);
            }
        }
    }

public:
    void OnEvent(FsmEvent* e)
    {
        if (m_pCurState) 
        {
            UINT uNewStateID = m_pCurState->GetStateID();
            m_pCurState->OnEvent(e, uNewStateID);

            if (uNewStateID != m_pCurState->GetStateID()) 
            {
                IState* pOldState = m_pCurState;

                m_pCurState->Leave();
                m_pCurState = GetState(uNewStateID);
                m_pCurState->Enter(pOldState);
            }
        }
    }

protected:
    std::map<UINT, IState*> m_StateMap;   ///< 状态表

    IState* m_pCurState;                  ///< 当前状态指针
    IState* m_pExitState;
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
        this->SetStateName(#State_Name);                        \
    }

///< 开始定义OnEvent
#define  BEGIN_ON_EVENT()                                       \
public: void OnEvent(Event* e, UINT& uNewStateID)               \
{

///< 结束定义OnEvent
#define  END_ON_EVENT                                           \
}

///< 开始定义Leave
#define  BEGIN_LEAVE()                                          \
public: void Leave()                                            \
{

///< 结束定义Leave
#define  END_LEAVE                                              \
}

///< 开始定义Enter
#define  BEGIN_ENTER()                                          \
public: void Enter(IState* s)                                   \
{

///< 结束定义Enter
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
    IState* s = NULL;

///< 注册状态
#define  REGISTER_STATE(State_Name)                             \
    this->AddState(new State_Name);                             \

///< 注册初始状态
#define REGISTER_INIT_STATE(State_Name)                         \
    s = new State_Name;                                         \
    if (s) {                                                    \
        this->AddState(s);                                      \
        this->SetInitState(s);                                  \
    }

///< 注册结束状态
#define REGISTER_EXIT_STATE(State_Name)                         \
    s = new State_Name;                                         \
    this->AddState(s);                                          \
    this->SetExitState(s);

///< 结束注册状态
#define  END_REGISTER_STATE                                     \
}

///< 结束fsm定义
#define  END_FSM                                                \
};


/*
///< 状态机的定义
BEGIN_FSM(ClientAppFsm)
    BEGIN_REGISTER_STATE()
        REGISTER_INIT_STATE(CQueryLogicState)
        REGISTER_STATE(CQueryNewVerState)
        REGISTER_STATE(COnlineState)
    END_REGISTER_STATE
END

ClientAppFsm    m_fsm;
*/


}

}}