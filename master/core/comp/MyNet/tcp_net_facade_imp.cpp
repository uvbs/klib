#include "StdAfx.h"
#include "tcp_net_facade_imp.h"

#include "icombiner.h"
#include "net_conn.h"
#include "inetwork_imp.h"
#include "icombiner_imp.h"
#include "inetpacket_mgr_imp.h"
#include "dispatcher_handler.h"
#include "inet_conn_mgr_imp.h"

tcp_net_facade_imp::tcp_net_facade_imp(void)
{
    icombiner_        = NULL;
    inetwork_         = NULL;
    dispatch_handler_ = NULL;
    net_packet_mgr_   = NULL;
    net_conn_mgr_     = NULL;
    init_success_     = false;

}

tcp_net_facade_imp::~tcp_net_facade_imp(void)
{
}

bool tcp_net_facade_imp::set_icombiner(icombiner* pCombiner) 
{
    _ASSERT(pCombiner);
    guard helper(mutex_);

    icombiner_ = pCombiner;
    return icombiner_ != NULL;
}

bool tcp_net_facade_imp::set_dispatch_handler(dispatcher_handler* pHandler) 
{
    _ASSERT(pHandler);
    guard helper(mutex_);

    dispatch_handler_ = pHandler;
    return dispatch_handler_ != NULL;
}

bool tcp_net_facade_imp::set_net_conn_mgr(inet_conn_mgr* pMgr) 
{
    _ASSERT(pMgr);
    guard helper(mutex_);

    net_conn_mgr_ = pMgr;
    return net_conn_mgr_ != NULL;
}

bool tcp_net_facade_imp::init_client()
{
    if (init_success_)
    {
        return init_success_;
    }

    inetwork_ = new inetwork_imp;
    _ASSERT(inetwork_);
    inetwork_->init_network(this);
    inetwork_->run_network();

    // 初始化默认的处理器
    icombiner_ = new icombiner_imp;
    net_packet_mgr_ = new inetpacket_mgr_imp;
    net_conn_mgr_ = new inet_conn_mgr_imp;

    init_success_ = (icombiner_ && net_packet_mgr_ && net_conn_mgr_);

    return init_success_;
}

bool tcp_net_facade_imp::add_event_handler(inet_event_handler* handler) 
{
    guard helper(mutex_);

#ifdef _DEBUG
    INetEventHandlerListType::const_iterator itr;
    itr = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) 
    {
        if ((*itr) == handler) {
            _ASSERT(FALSE);
        }
    }
#endif

    net_event_list_.push_back(handler);
    return true;
}

bool tcp_net_facade_imp::del_event_handler(inet_event_handler* handler) 
{
    guard helper(mutex_);

    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr)
    {
        if (handler == (*itr)) {
            net_event_list_.erase(itr);
            return true;
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
bool tcp_net_facade_imp::on_connect(net_conn* pConn, bool bConnected/* = true*/) 
{
    if (bConnected) 
    {
        //const char *msg = "GET / HTTP/1.1\r\nAccept: */*\r\nHost: www.baidu.com\r\n\r\n";
        //inetwork_->post_write(pConn, msg, strlen(msg));
    }
    else 
    {
        //TRACE(TEXT("连接失败!\r\n"));
    }

#ifdef _DEBUG
    if (net_conn_mgr_) 
    {
        if (net_conn_mgr_->is_exist_conn(pConn)) 
        {
            _ASSERT(FALSE && "连接添加错误，设计错误");
        }
    }
#endif

    //将连接添加到连接管理器里
    if (net_conn_mgr_) {
        net_conn_mgr_->add_conn(pConn);
    }

    // 通知上层处理连接事件
    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) 
    {
        (*itr)->on_connect(pConn, bConnected);
    }

    return true;
}

bool tcp_net_facade_imp::on_disconnect(net_conn* pConn) 
{
    //MyPrtLog("连接断开了...\r\n");

#ifdef _DEBUG
    if (net_conn_mgr_) 
    {
        if (!net_conn_mgr_->is_exist_conn(pConn)) 
        {
            // 这里可能是由于刚才连接上还没有添加到mgr里面，进行投递读请求就会出错;
            _ASSERT(FALSE && "连接关闭错误，设计错误");
            return true;
        }
    }
#endif

    //这里是先交给上层处理再释放
    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) 
    {
        (*itr)->on_disconnect(pConn);
    }

    // 删除连接
    if (net_conn_mgr_) 
    {
        net_conn_mgr_->del_conn(pConn);
    }

    // 释放所有的数据包
    if (net_packet_mgr_) 
    {
        net_packet_mgr_->free_conn_packets(pConn);
    }

    return true;
}

bool tcp_net_facade_imp::on_read(net_conn* pConn, const char* buff, size_t len)
{
    //MyPrtLog("有数据来啦Len: %d", len);
    _ASSERT(buff);
    int iPacketLen = 0;
    bool bIsCombined = false;

    pConn->write_recv_stream(buff, len);
    bIsCombined = icombiner_->IsIntactPacket(pConn->get_recv_stream(), iPacketLen);

    while (bIsCombined) 
    {
        //添加封包
        //MyPrtLog("已构成完整封包....\r\n");

        net_packet* pPacket = net_packet_mgr_->alloc_net_packet();
        if (NULL == pPacket) 
        {
            _ASSERT(FALSE);
            return false;
        }

        //TRACE(TEXT("封包生成..."));
        pPacket->pConn    = pConn;
        pPacket->bf_size_ = iPacketLen;
        if (pPacket->init_buff(iPacketLen)) {
            return false;
        }
        pConn->read_recv_stream(pPacket->get_buff(), iPacketLen);

        if (dispatch_handler_) 
        {
            dispatch_handler_->DispatchPacket(pPacket);
            net_packet_mgr_->free_net_packet(pPacket);
        }
        else 
        {
            net_packet_mgr_->add_packet(pPacket);
        }

        if (pConn->get_recv_length() <= 0)
        {
            break;
        }

        bIsCombined = icombiner_->IsIntactPacket(pConn->get_recv_stream(), iPacketLen);
    }
    
    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) {
        (*itr)->on_read(pConn, buff, len);
    }

    return true;
}

bool tcp_net_facade_imp::on_write(net_conn* pConn) 
{
    //MyPrtLog("写数据完毕..\r\n");

    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) {
        (*itr)->on_write(pConn);
    }

    return true;
}

bool tcp_net_facade_imp::on_accept(net_conn* pListen, net_conn* pNewConn, bool bSuccess/* = true*/) 
{
    if (net_conn_mgr_ && bSuccess) {
#ifdef _DEBUG
        if (net_conn_mgr_->is_exist_conn(pNewConn)) {
            _ASSERT(FALSE && "重复添加连接，这里设计出错!");
        }
#endif
        net_conn_mgr_->add_conn(pNewConn);
    }

    if (bSuccess) {
        //MyPrtLog("连接来啦...");
    }
    else {
        //MyPrtLog("接收连接失败...");
    }

    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) {
        (*itr)->on_accept(pListen, pNewConn, bSuccess);
    }
    return true;
}