#include "StdAfx.h"
#include "INetClientImp.h"

#include "ICombiner.h"
#include "INetConnection.h"
#include "INetworkImp.h"
#include "ICombinerImp.h"
#include "INetPacketMgrImp.h"
#include "IDispatchHandler.h"
#include "INetConnectionMgrImp.h"

INetTcpClientImp::INetTcpClientImp(void)
{
    m_pICombiner = NULL;
    m_pINetwork = NULL;
    m_pIDispatcher = NULL;
    m_pNetPacketMgr = NULL;
    m_pINetConnMgr = NULL;
    m_bInitSucc  =    false;

}

INetTcpClientImp::~INetTcpClientImp(void)
{
}

bool INetTcpClientImp::SetICombiner(ICombiner* pCombiner) 
{
    _ASSERT(pCombiner);
    auto_lock helper(m_cs);

    m_pICombiner = pCombiner;
    return m_pICombiner != NULL;
}

bool INetTcpClientImp::SetIDispatchHandler(IDispatchHandler* pHandler) 
{
    _ASSERT(pHandler);
    auto_lock helper(m_cs);

    m_pIDispatcher = pHandler;
    return m_pIDispatcher != NULL;
}

bool INetTcpClientImp::SetINetConnectionMgr(INetConnectionMgr* pMgr) 
{
    _ASSERT(pMgr);
    auto_lock helper(m_cs);

    m_pINetConnMgr = pMgr;
    return m_pINetConnMgr != NULL;
}

bool INetTcpClientImp::InitClient()
{
    if (m_bInitSucc)
    {
        return m_bInitSucc;
    }

    m_pINetwork = new INetNetworkImp;
    _ASSERT(m_pINetwork);
    m_pINetwork->InitNetwork();
    m_pINetwork->SetNetEventHandler(this);
    m_pINetwork->RunNetwork();

    // 初始化默认的处理器
    m_pICombiner = new ICombinerImp;
    m_pNetPacketMgr = new INetPacketMgrImp;
    m_pINetConnMgr = new INetConnectionMgrImp;

    m_bInitSucc = (m_pICombiner && m_pNetPacketMgr && m_pINetConnMgr);

    return m_bInitSucc;
}

bool INetTcpClientImp::AddEventHandler(INetEventHandler* handler) 
{
    auto_lock helper(m_cs);

#ifdef _DEBUG
    INetEventHandlerListType::const_iterator itr;
    itr = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) 
    {
        if ((*itr) == handler) {
            _ASSERT(FALSE);
        }
    }
#endif

    m_INetEventList.push_back(handler);
    return true;
}

bool INetTcpClientImp::RemoveEventHandler(INetEventHandler* handler) 
{
    auto_lock helper(m_cs);

    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr)
    {
        if (handler == (*itr)) {
            m_INetEventList.erase(itr);
            return true;
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
bool INetTcpClientImp::OnConnect(INetConnection* pConn, bool bConnected/* = true*/) 
{
    if (bConnected) {
        //const char *msg = "GET / HTTP/1.1\r\nAccept: */*\r\nHost: www.baidu.com\r\n\r\n";
        //m_pINetwork->PostWrite(pConn, msg, strlen(msg));
    }
    else {
        //TRACE(TEXT("连接失败!\r\n"));
    }

#ifdef _DEBUG
    if (m_pINetConnMgr) {
        if (m_pINetConnMgr->is_exist_conn(pConn)) {
            _ASSERT(FALSE && "连接添加错误，设计错误");
        }
    }
#endif

    //将连接添加到连接管理器里
    if (m_pINetConnMgr) {
        m_pINetConnMgr->add_conn(pConn);
    }

    //通知连接事件
    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) {
        (*itr)->OnConnect(pConn, bConnected);
    }

    return true;
}

bool INetTcpClientImp::OnDisConnect(INetConnection* pConn) 
{
    //MyPrtLog("连接断开了...\r\n");

#ifdef _DEBUG
    if (m_pINetConnMgr) {
        if (!m_pINetConnMgr->is_exist_conn(pConn)) {
            _ASSERT(FALSE && "连接关闭错误，设计错误");
            // 这里可能是由于刚才连接上还没有添加到mgr里面，进行投递读请求就会出错;
            return true;
        }
    }
#endif

    //这里是先交给上层处理再释放
    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) {
        (*itr)->OnDisConnect(pConn);
    }

    if (m_pINetConnMgr) {
        m_pINetConnMgr->rmv_conn(pConn);
    }
    if (m_pNetPacketMgr) {
        m_pNetPacketMgr->FreeConnPacket(pConn);
    }

    return true;
}

bool INetTcpClientImp::OnRead(INetConnection* pConn, const char* buff, size_t len)
{
    //MyPrtLog("有数据来啦Len: %d", len);
    _ASSERT(buff);
    int iPacketLen = 0;
    bool bIsCombined = false;

    pConn->AddStream(buff, len);
    bIsCombined = m_pICombiner->IsIntactPacket(pConn->GetBuff(),
        pConn->GetDataLen(),
        iPacketLen);

    while (bIsCombined) 
    {
        //添加封包
        //MyPrtLog("已构成完整封包....\r\n");

        NetPacket* pPacket = m_pNetPacketMgr->AllocNetPacket();
        if (NULL == pPacket) 
        {
            _ASSERT(FALSE);
            return false;
        }

        //TRACE(TEXT("封包生成..."));
        pPacket->pConn = pConn;
        pPacket->datalen = iPacketLen;
        pConn->GetStream(pPacket->buff, iPacketLen);

        if (m_pIDispatcher) 
        {
            m_pIDispatcher->DispatchPacket(pPacket);
            m_pNetPacketMgr->FreeNetPacket(pPacket);
        }
        else 
        {
            m_pNetPacketMgr->AddPacket(pPacket);
        }

        if (pConn->GetDataLen() <= 0)
        {
            break;
        }

        bIsCombined = m_pICombiner->IsIntactPacket(pConn->GetBuff(), 
            pConn->GetDataLen(), 
            iPacketLen);
    }

#ifdef _DEBUG
    pConn->GetBuff()[pConn->GetDataLen()] = '\0';
    //MyPrtLog("剩余数据>>长度 = %d \r\n", pConn->GetDataLen());
#endif  

    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) {
        (*itr)->OnRead(pConn, buff, len);
    }

    return true;
}

bool INetTcpClientImp::OnWrite(INetConnection* pConn) 
{
    //MyPrtLog("写数据完毕..\r\n");

    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) {
        (*itr)->OnWrite(pConn);
    }

    return true;
}

bool INetTcpClientImp::OnAccept(INetConnection* pListen, INetConnection* pNewConn, bool bSuccess/* = true*/) 
{
    if (m_pINetConnMgr && bSuccess) {
#ifdef _DEBUG
        if (m_pINetConnMgr->is_exist_conn(pNewConn)) {
            _ASSERT(FALSE && "重复添加连接，这里设计出错!");
        }
#endif
        m_pINetConnMgr->add_conn(pNewConn);
    }

    if (bSuccess) {
        //MyPrtLog("连接来啦...");
    }
    else {
        //MyPrtLog("接收连接失败...");
    }

    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) {
        (*itr)->OnAccept(pListen, pNewConn, bSuccess);
    }
    return true;
}