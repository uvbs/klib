#include "StdAfx.h"
#include "tcpclient_imp.h"

#include "icombiner.h"
#include "net_conn.h"
#include "INetworkImp.h"
#include "icombiner_imp.h"
#include "inetpacket_mgr_imp.h"
#include "dispatcher_handler.h"
#include "inet_conn_mgr_imp.h"

tcpclient_imp::tcpclient_imp(void)
{
    m_pICombiner = NULL;
    m_pINetwork = NULL;
    m_pIDispatcher = NULL;
    m_pNetPacketMgr = NULL;
    m_pINetConnMgr = NULL;
    m_bInitSucc  =    false;

}

tcpclient_imp::~tcpclient_imp(void)
{
}

bool tcpclient_imp::SetICombiner(icombiner* pCombiner) 
{
    _ASSERT(pCombiner);
    auto_lock helper(m_cs);

    m_pICombiner = pCombiner;
    return m_pICombiner != NULL;
}

bool tcpclient_imp::SetIDispatchHandler(dispatcher_handler* pHandler) 
{
    _ASSERT(pHandler);
    auto_lock helper(m_cs);

    m_pIDispatcher = pHandler;
    return m_pIDispatcher != NULL;
}

bool tcpclient_imp::SetINetConnectionMgr(inet_conn_mgr* pMgr) 
{
    _ASSERT(pMgr);
    auto_lock helper(m_cs);

    m_pINetConnMgr = pMgr;
    return m_pINetConnMgr != NULL;
}

bool tcpclient_imp::InitClient()
{
    if (m_bInitSucc)
    {
        return m_bInitSucc;
    }

    m_pINetwork = new inetwork_imp;
    _ASSERT(m_pINetwork);
    m_pINetwork->init_network();
    m_pINetwork->set_net_event_handler(this);
    m_pINetwork->run_network();

    // 初始化默认的处理器
    m_pICombiner = new icombiner_imp;
    m_pNetPacketMgr = new inetpacket_mgr_imp;
    m_pINetConnMgr = new inet_conn_mgr_imp;

    m_bInitSucc = (m_pICombiner && m_pNetPacketMgr && m_pINetConnMgr);

    return m_bInitSucc;
}

bool tcpclient_imp::AddEventHandler(inet_event_handler* handler) 
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

bool tcpclient_imp::RemoveEventHandler(inet_event_handler* handler) 
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
bool tcpclient_imp::OnConnect(net_conn* pConn, bool bConnected/* = true*/) 
{
    if (bConnected) {
        //const char *msg = "GET / HTTP/1.1\r\nAccept: */*\r\nHost: www.baidu.com\r\n\r\n";
        //m_pINetwork->post_write(pConn, msg, strlen(msg));
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

bool tcpclient_imp::OnDisConnect(net_conn* pConn) 
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
        m_pINetConnMgr->del_conn(pConn);
    }
    if (m_pNetPacketMgr) {
        m_pNetPacketMgr->free_conn_packets(pConn);
    }

    return true;
}

bool tcpclient_imp::OnRead(net_conn* pConn, const char* buff, size_t len)
{
    //MyPrtLog("有数据来啦Len: %d", len);
    _ASSERT(buff);
    int iPacketLen = 0;
    bool bIsCombined = false;

    pConn->write_recv_stream(buff, len);
    bIsCombined = m_pICombiner->IsIntactPacket(pConn->get_recv_stream(), iPacketLen);

    while (bIsCombined) 
    {
        //添加封包
        //MyPrtLog("已构成完整封包....\r\n");

        net_packet* pPacket = m_pNetPacketMgr->alloc_net_packet();
        if (NULL == pPacket) 
        {
            _ASSERT(FALSE);
            return false;
        }

        //TRACE(TEXT("封包生成..."));
        pPacket->pConn = pConn;
        pPacket->datalen = iPacketLen;
        pConn->read_recv_stream(pPacket->buff, iPacketLen);

        if (m_pIDispatcher) 
        {
            m_pIDispatcher->DispatchPacket(pPacket);
            m_pNetPacketMgr->free_net_packet(pPacket);
        }
        else 
        {
            m_pNetPacketMgr->add_packet(pPacket);
        }

        if (pConn->get_recv_length() <= 0)
        {
            break;
        }

        bIsCombined = m_pICombiner->IsIntactPacket(pConn->get_recv_stream(), iPacketLen);
    }
    
    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) {
        (*itr)->OnRead(pConn, buff, len);
    }

    return true;
}

bool tcpclient_imp::OnWrite(net_conn* pConn) 
{
    //MyPrtLog("写数据完毕..\r\n");

    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) {
        (*itr)->OnWrite(pConn);
    }

    return true;
}

bool tcpclient_imp::OnAccept(net_conn* pListen, net_conn* pNewConn, bool bSuccess/* = true*/) 
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