#pragma once

#include "tcpclient.h"
#include <vector>


class tcpclient_imp : public tcpclient
{
public:
    tcpclient_imp(void);
    ~tcpclient_imp(void);

public:
    //////////////////////////////////////////////////////////////////////////
    //初始设置接口
    virtual bool SetICombiner(icombiner* pCombiner) ;               //设置分包处理接口
    virtual bool SetIDispatchHandler(dispatcher_handler* pHandler) ;  //设置分派接口,如果不设置这个，则封包进入封包管理器中
    virtual bool SetINetConnectionMgr(inet_conn_mgr* pMgr) ;    //连接管理器
    virtual bool InitClient();                                      //初始化客户端

    //////////////////////////////////////////////////////////////////////////
    //获取其它接口
    virtual inetpacket_mgr* GetNetPacketMgr() { return m_pNetPacketMgr;}       //返回网络封包管理接口
    virtual inetwork*   GetNetwork() { return m_pINetwork; }					//返回网络层接口
    virtual inet_conn_mgr* GetINetConnectionMgr() { return m_pINetConnMgr; }	//返回连接管理接口

    //////////////////////////////////////////////////////////////////////////
    //事件处理接口
    virtual bool AddEventHandler(inet_event_handler* handler) ;
    virtual bool RemoveEventHandler(inet_event_handler* handler) ;

protected:
    virtual bool OnConnect(net_conn* pConn, bool bConnected = true) ;
    virtual bool OnDisConnect(net_conn* pConn) ;
    virtual bool OnRead(net_conn* pConn, const char* buff, size_t len);
    virtual bool OnWrite(net_conn* pConn) ;
    virtual bool OnAccept(net_conn* pListen, net_conn* pNewConn, bool bSuccess = true) ;

protected:

protected:
    bool                m_bInitSucc;                //表示是否初始化成功
    icombiner*          m_pICombiner;               //包完整性判断,基于应用层的协议
    dispatcher_handler*   m_pIDispatcher;             //派遣接口
    inetwork*           m_pINetwork;                //网络接口
    inetpacket_mgr*      m_pNetPacketMgr;            //封包管理器
    inet_conn_mgr*  m_pINetConnMgr;             //网络连接管理器


    auto_cs     m_cs;                //同步结构

    typedef std::vector<inet_event_handler*> INetEventHandlerListType;
    INetEventHandlerListType m_INetEventList;    //事件处理器列表
};