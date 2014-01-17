#pragma once

#include "tcp_net_facade.h"
#include <vector>


class tcp_net_facade_imp : public tcp_net_facade
{
public:
    tcp_net_facade_imp(void);
    ~tcp_net_facade_imp(void);

public:
    //////////////////////////////////////////////////////////////////////////
    //初始设置接口
    virtual bool set_icombiner(icombiner* pCombiner) ;               //设置分包处理接口
    virtual bool set_dispatch_handler(dispatcher_handler* pHandler) ;  //设置分派接口,如果不设置这个，则封包进入封包管理器中
    virtual bool set_net_conn_mgr(inet_conn_mgr* pMgr) ;    //连接管理器
    virtual bool init_client();                                      //初始化客户端

    //////////////////////////////////////////////////////////////////////////
    //获取其它接口
    virtual inetpacket_mgr* get_net_packet_mgr() { return m_pNetPacketMgr;}         //返回网络封包管理接口
    virtual inetwork*       get_network() { return m_pINetwork; }					//返回网络层接口
    virtual inet_conn_mgr*  get_net_conn_mgr() { return m_pINetConnMgr; }	        //返回连接管理接口

    //////////////////////////////////////////////////////////////////////////
    //事件处理接口
    virtual bool add_event_handler(inet_event_handler* handler) ;
    virtual bool del_event_handler(inet_event_handler* handler) ;

protected:
    virtual bool OnConnect(net_conn* pConn, bool bConnected = true) ;
    virtual bool OnDisConnect(net_conn* pConn) ;
    virtual bool OnRead(net_conn* pConn, const char* buff, size_t len);
    virtual bool OnWrite(net_conn* pConn) ;
    virtual bool OnAccept(net_conn* pListen, net_conn* pNewConn, bool bSuccess = true) ;

protected:

protected:
    bool                    m_bInitSucc;                    //表示是否初始化成功
    icombiner*              m_pICombiner;                   //包完整性判断,基于应用层的协议
    dispatcher_handler*     m_pIDispatcher;                 //派遣接口
    inetwork*               m_pINetwork;                    //网络接口
    inetpacket_mgr*         m_pNetPacketMgr;                //封包管理器
    inet_conn_mgr*          m_pINetConnMgr;                 //网络连接管理器

    auto_cs     m_cs;                //同步结构

    typedef std::vector<inet_event_handler*> INetEventHandlerListType;
    INetEventHandlerListType m_INetEventList;    //事件处理器列表
};