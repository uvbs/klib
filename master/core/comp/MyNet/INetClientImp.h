#pragma once

#include "INetClient.h"
#include <vector>


class INetTcpClientImp : public INetTcpClient
{
public:
    INetTcpClientImp(void);
    ~INetTcpClientImp(void);

public:
    //////////////////////////////////////////////////////////////////////////
    //初始设置接口
    virtual bool SetICombiner(ICombiner* pCombiner) ;               //设置分包处理接口
    virtual bool SetIDispatchHandler(IDispatchHandler* pHandler) ;  //设置分派接口,如果不设置这个，则封包进入封包管理器中
    virtual bool SetINetConnectionMgr(INetConnectionMgr* pMgr) ;    //连接管理器
    virtual bool InitClient();                                      //初始化客户端

    //////////////////////////////////////////////////////////////////////////
    //获取其它接口
    virtual INetPacketMgr* GetNetPacketMgr() { return m_pNetPacketMgr;}       //返回网络封包管理接口
    virtual INetNetwork*   GetNetwork() { return m_pINetwork; }					//返回网络层接口
    virtual INetConnectionMgr* GetINetConnectionMgr() { return m_pINetConnMgr; }	//返回连接管理接口

    //////////////////////////////////////////////////////////////////////////
    //事件处理接口
    virtual bool AddEventHandler(INetEventHandler* handler) ;
    virtual bool RemoveEventHandler(INetEventHandler* handler) ;

protected:
    virtual bool OnConnect(INetConnection* pConn, bool bConnected = true) ;
    virtual bool OnDisConnect(INetConnection* pConn) ;
    virtual bool OnRead(INetConnection* pConn, const char* buff, size_t len);
    virtual bool OnWrite(INetConnection* pConn) ;
    virtual bool OnAccept(INetConnection* pListen, INetConnection* pNewConn, bool bSuccess = true) ;

protected:

protected:
    bool                m_bInitSucc;                //表示是否初始化成功
    ICombiner*          m_pICombiner;               //包完整性判断,基于应用层的协议
    IDispatchHandler*   m_pIDispatcher;             //派遣接口
    INetNetwork*           m_pINetwork;                //网络接口
    INetPacketMgr*      m_pNetPacketMgr;            //封包管理器
    INetConnectionMgr*  m_pINetConnMgr;             //网络连接管理器


    auto_cs     m_cs;                //同步结构

    typedef std::vector<INetEventHandler*> INetEventHandlerListType;
    INetEventHandlerListType m_INetEventList;    //事件处理器列表
};