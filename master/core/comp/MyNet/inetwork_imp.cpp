#include "StdAfx.h"
#include "inetwork_imp.h"
#include <process.h>

#include "inet_tcp_handler.h"
#include "net_conn.h"
#include <net/addr_resolver.h>
#include <net/winsockInit.h>

#include <core/scope_guard.h>

using namespace klib::core;

#include <BaseTsd.h>
#include <MSWSock.h>

#define  MAX_BUF_SIZE 1024

#if (_MSC_VER == 1200)
typedef
    BOOL
    (PASCAL FAR * LPFN_CONNECTEX) (
    SOCKET s,
    const struct sockaddr FAR *name,
    int namelen,
    PVOID lpSendBuffer,
    DWORD dwSendDataLength,
    LPDWORD lpdwBytesSent,
    LPOVERLAPPED lpOverlapped
    );

#define WSAID_CONNECTEX \
{0x25a207b9,0xddf3,0x4660,{0x8e,0xe9,0x76,0xe5,0x8c,0x74,0x06,0x3e}}

#endif

klib::net::winsock_init g_winsock_init_;

CObjectPool<worker_context, 10000, 1000> g_worker_contex_pool_;


//----------------------------------------------------------------------
//
bool network_worker::execute(worker_context*& ctx)
{
    ON_SCOPE_EXIT(
        g_worker_contex_pool_.Free(ctx);
    );

    if (ctx->ctx_type_ == context_recv_ctx) 
    {
        return this->on_recv_ctx(ctx);
    }
    else if (ctx->ctx_type_ == context_send_ctx) 
    {
        return this->on_send_ctx(ctx);
    }

    KLIB_ASSERT(FALSE);
    return true;
}

bool network_worker::on_recv_ctx(worker_context*& ctx)
{
    auto dwByteTransfered = ctx->recv_info.dwByteTransfered;
    auto lpOverlapped = ctx->recv_info.lpOverlapped;
    auto bResult = ctx->recv_info.bResult;
    auto pConn = ctx->pConn;
    inetwork_imp* pimp = ctx->network_;

    ON_SCOPE_EXIT(
        if (NULL != lpOverlapped) 
        {
            pimp->net_overlapped_pool_.Free(lpOverlapped);  //释放lpOverlapped结构，每次有请求的时候重新获取
        }            
    );

    if (bResult) 
    {
        if(-1 == dwByteTransfered && NULL == lpOverlapped) 
        {
            return true;
        }

        switch(lpOverlapped->operate_type_ )
        {
        case OP_READ:
            pimp->on_read(pConn, (const char*) lpOverlapped->recv_buff_, dwByteTransfered);
            break;

        case OP_WRITE:
            pimp->on_write(pConn, dwByteTransfered);
            break;

        case OP_ACCEPT:
            pimp->on_accept(pConn, (net_conn*) lpOverlapped->pend_data_);  // AcceptConn
            break;

        case OP_CONNECT:
            pimp->on_connect(pConn, true);
            break;
        }

    }
    else 
    {
        // 更新投递计数
        if (lpOverlapped->operate_type_ == OP_READ) 
        {
            pConn->dec_post_read_count();
        }
        else if (lpOverlapped->operate_type_ == OP_WRITE) 
        {
            pConn->dec_post_write_count();
        }

        if (lpOverlapped->operate_type_ == OP_CONNECT) 
        {
            // 主动连接失败的时候，先关闭连接，通知上层处理，然后释放连接对象
            pConn->dis_connect();
            pimp->net_event_handler_->on_connect(pConn, false);

            pimp->net_conn_pool_.Free(pConn);
        }
        else if (lpOverlapped->operate_type_ == OP_ACCEPT) 
        {
            // 接受连接失败,释放连接
            net_conn* pListConn = pConn;                                     // 监听套接字
            net_conn* pAcceptConn = (net_conn*) lpOverlapped->pend_data_;    // 接收的连接
            pimp->post_accept(pListConn);                                    // 继续投递接受连接请求
            pimp->net_event_handler_->on_accept(pListConn, pAcceptConn, false);    // 通知上层接受连接失败

            pimp->net_conn_pool_.Free(pAcceptConn);                                   // 释放
        }
        else 
        {
            // 处理其它操作（如需要释放连接）
            pConn->dis_connect();
            pimp->check_and_disconnect(pConn);
            pimp->net_conn_pool_.Free(pConn);
        }
    } //if (bResult) {

    return true;
}

bool network_worker::on_send_ctx(worker_context*& ctx)
{
    ON_SCOPE_EXIT(
        if (ctx->send_info.buff_ptr_) 
        {
            // 考虑更好的发送方式
            delete [] ctx->send_info.buff_ptr_;
        }
    );

    auto pimp  = ctx->network_;
    auto pConn = ctx->pConn;
    auto buff  = ctx->send_info.buff_ptr_;
    auto len   = ctx->send_info.buff_len_;

    // 先将要发送的数据放入流中(支持多线程)
    pConn->write_send_stream(buff, len);

    //@todo 需要计算速率，看是否需要控制速率
    if (pConn->get_post_write_count() > 0) {
        return true;
    }

    // 一定有数据可以发送
    KLIB_ASSERT(pConn->get_send_length() > 0);

    // 先发送流中的数据
    auto seg_ptr = pConn->get_send_stream().get_read_seg_ptr();
    auto seg_len = pConn->get_send_stream().get_read_seg_len();

    return pimp->post_placement_write(pConn, (char*)seg_ptr, seg_len);
}

//----------------------------------------------------------------------
//
inetwork_imp::inetwork_imp(void)
{
    m_lpfnAcceptEx = NULL;
    hiocp_ = INVALID_HANDLE_VALUE;

    m_bstop = false;
}

inetwork_imp::~inetwork_imp(void)
{
}

bool inetwork_imp::init_network(inet_tcp_handler* handler, 
    size_t thread_num/* = 1*/,
    size_t worker_num/* = 7*/)
{
    // 创建完成端口
    hiocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,(ULONG_PTR)0, 0);
    _ASSERT(hiocp_);
    if (NULL == hiocp_) 
        return false;
    
    // 设置事件处理器
    _ASSERT(handler);
    net_event_handler_ = handler;

    init_threads(thread_num);

    init_workers(worker_num);

    return true;
}

bool inetwork_imp::try_write(net_conn* pconn, const char* buff, size_t len)
{
    //@todo 转为导步的发送方式(缓冲区最好支持引用计数)
    char* pbuff = new char[len];
    if (NULL == pbuff) {
        return false;
    }

    memcpy(pbuff, buff, len);
    worker_context* ctx = g_worker_contex_pool_.Alloc();
    ctx->network_ = this;
    ctx->pConn = pconn;
    ctx->ctx_type_ = context_send_ctx;

    ctx->send_info.buff_ptr_ = pbuff;
    ctx->send_info.buff_len_ = len;
    return get_worker(pconn)->send(ctx);
}

net_conn* inetwork_imp::try_listen(USHORT local_port) 
{
    net_conn* pconn =  this->create_listen_conn(local_port);
    if (NULL == pconn) 
        return NULL;

    this->post_accept(pconn);
    return pconn;
}

net_conn* inetwork_imp::try_connect(const char* addr, USHORT uport, void* bind_key) 
{
    if (NULL == addr) {
        return NULL;
    }

    net_conn* pconn = net_conn_pool_.Alloc();
    if (NULL == pconn) {
        return NULL;
    }

    pconn->set_bind_key(bind_key);
    pconn->set_peer_addr_str(addr);
    pconn->set_peer_port(uport);
    this->post_connection(pconn);

    return pconn;
}

//----------------------------------------------------------------------
// 
net_conn* inetwork_imp::post_accept(net_conn* pListenConn) 
{
    HANDLE hResult;
    DWORD dwBytes = 0;
    net_overLapped* lpOverlapped = get_net_overlapped();
    if (lpOverlapped == NULL) {
        return false;
    }

    SOCKET sockAccept = WSASocket(AF_INET,SOCK_STREAM, IPPROTO_TCP, NULL, NULL,WSA_FLAG_OVERLAPPED);
    if (sockAccept == INVALID_SOCKET) {
        net_overlapped_pool_.Free(lpOverlapped);
        //TRACE(TEXT("Error at socket(): %ld\n"));
        return NULL;
    }

    if (NULL == m_lpfnAcceptEx) {
        GUID GuidAcceptEx = WSAID_ACCEPTEX;
        DWORD irt = WSAIoctl(sockAccept,
            SIO_GET_EXTENSION_FUNCTION_POINTER,
            &GuidAcceptEx,
            sizeof(GuidAcceptEx),
            &m_lpfnAcceptEx,
            sizeof(m_lpfnAcceptEx),
            &dwBytes,
            NULL, 
            NULL);

        if (irt == SOCKET_ERROR)
        {
            closesocket(sockAccept);
            net_overlapped_pool_.Free(lpOverlapped);
            //TRACE(TEXT("fail at WSAIoctl-AcceptEx,Error Code:%d\n"));
            return NULL;
        }
    }

    net_conn* pNewConn = net_conn_pool_.Alloc();
    if (NULL == pNewConn) {
        closesocket(sockAccept);
        net_overlapped_pool_.Free(lpOverlapped);
        return NULL;
    }

    hResult = CreateIoCompletionPort((HANDLE)sockAccept, hiocp_, (ULONG_PTR)pNewConn, 0);
    if (NULL == hResult) {
        closesocket(sockAccept);
        net_overlapped_pool_.Free(lpOverlapped);
        net_conn_pool_.Free(pNewConn);
        _ASSERT(FALSE);
        return NULL;
    }

    // 以可以调用getpeername获取套接字信息
    BOOL bSetUpdate = TRUE;
    setsockopt(sockAccept,
        SOL_SOCKET,
        SO_UPDATE_ACCEPT_CONTEXT,
        (char *)& bSetUpdate,
        sizeof(bSetUpdate) );

    pNewConn->get_socket() = sockAccept;
    lpOverlapped->operate_type_ = OP_ACCEPT;
    lpOverlapped->pend_data_ = (void*) pNewConn;  // 这里比较特殊，和其它情况不同
    lpOverlapped->hEvent = NULL;

    BOOL brt = m_lpfnAcceptEx(pListenConn->get_socket(),
        sockAccept,
        lpOverlapped->recv_buff_, 
        0,//lpToverlapped->dwBufSize,
        sizeof(sockaddr_in) + 16, 
        sizeof(sockaddr_in) + 16, 
        &(lpOverlapped->transfer_bytes_), 
        (LPOVERLAPPED)lpOverlapped);


    if(brt == FALSE && WSA_IO_PENDING != WSAGetLastError()) 
    {
        closesocket(sockAccept);
        net_overlapped_pool_.Free(lpOverlapped);
        net_conn_pool_.Free(pNewConn);

        DWORD dwErrorCode = WSAGetLastError();
        //TRACE(TEXT("fail at lpfnAcceptEx,error code:%d\n"), dwErrorCode);
        return NULL;
    }
    else if (WSA_IO_PENDING == WSAGetLastError()) 
    {
        //成功
        return pNewConn;
    }

    //成功
    return pNewConn;
}

bool inetwork_imp::post_connection(net_conn* pConn) 
{
    _ASSERT(pConn);

    UINT32  uPeerAddr = 0;
    klib::net::addr_resolver ip_resolver_(pConn->get_peer_addr_str());
    if (ip_resolver_.size() < 0)  
    {
        return false;
    }
    uPeerAddr = ip_resolver_.at(0);

    SOCKET& sock = pConn->get_socket();
    sock = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
    if (sock == INVALID_SOCKET) 
    {
        _ASSERT(FALSE);  // DWORD dwErr = WSAGetLastError();
        _ASSERT(!"创建套接字失败!!!");
        return false;
    }

    sockaddr_in local_addr;
    ZeroMemory(&local_addr, sizeof(sockaddr_in));
    local_addr.sin_family = AF_INET;
    int iret = ::bind(sock, (sockaddr*)(&local_addr), sizeof(sockaddr_in));
    if (SOCKET_ERROR == iret) 
    {
        closesocket(sock);
        _ASSERT(!"绑定端口失败!!!");
        return false;
    }

    HANDLE hResult = CreateIoCompletionPort((HANDLE)sock,hiocp_, (ULONG_PTR)pConn,0);
    if (NULL == hResult) 
    {
        closesocket(sock);
        _ASSERT(FALSE);
        return false;
    }

    LPFN_CONNECTEX m_lpfnConnectEx = NULL;
    DWORD dwBytes = 0;
    GUID GuidConnectEx = WSAID_CONNECTEX;

    // 重点，获得ConnectEx函数的指针
    if (SOCKET_ERROR == WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
        &GuidConnectEx, sizeof (GuidConnectEx),
        &m_lpfnConnectEx, sizeof (m_lpfnConnectEx), &dwBytes, 0, 0))
    {
        closesocket(sock);
        //TRACE( TEXT("WSAIoctl is failed. Error code = %d"));//, WSAGetLastError());
        _ASSERT(FALSE);
        return FALSE;
    }

    net_overLapped *pmyoverlapped = get_net_overlapped(); // socket和I/O通讯的载体
    if (NULL == pmyoverlapped) 
    {
        _ASSERT(FALSE && "TODO 这里要释放资源嘛");
        return false;
    }
    pmyoverlapped->operate_type_ = OP_CONNECT;  // 设置请求类型，得到I/O结果时根据此                                   //                                         //来识别请求类型
    pmyoverlapped->hEvent = NULL;  // 非事件模型

    // 设置连接目标地址
    sockaddr_in addrPeer;  
    ZeroMemory(&addrPeer, sizeof(sockaddr_in));
    addrPeer.sin_family = AF_INET;
    addrPeer.sin_addr.s_addr = uPeerAddr;
    addrPeer.sin_port = htons( pConn->get_peer_port() );

    int nLen = sizeof(addrPeer);
    PVOID lpSendBuffer = NULL;
    DWORD dwSendDataLength = 0;
    DWORD dwBytesSent = 0;

    //重点
    BOOL bResult = m_lpfnConnectEx(sock,
        (sockaddr*)&addrPeer,  // [in] 对方地址
        nLen,  				// [in] 对方地址长度
        lpSendBuffer,		// [in] 连接后要发送的内容，这里不用
        dwSendDataLength,	// [in] 发送内容的字节数 ，这里不用
        &dwBytesSent,		// [out] 发送了多少个字节，这里不用
        (OVERLAPPED*)pmyoverlapped); // [in] 这东西复杂，下一篇有详解

    if (!bResult )  // 返回值处理
    {
        if( WSAGetLastError() != ERROR_IO_PENDING) 
        { 
            // 调用失败
            closesocket(sock);
            _ASSERT(FALSE);
            return FALSE;
        }
        else 
        {//操作未决（正在进行中…）
            //TRACE(TEXT("WSAGetLastError() == ERROR_IO_PENDING\n"));// 操作正在进行中
        }
    }

    return true;
}

bool inetwork_imp::post_read(net_conn* pConn) 
{
    DWORD dwRecvedBytes = 0;
    DWORD dwFlag = 0;
    DWORD transfer_bytes_ = 0;
    net_overLapped* pMyoverlapped = get_net_overlapped();
    if (NULL == pMyoverlapped) 
    {
        _ASSERT(pMyoverlapped);
        return false;
    }

    pMyoverlapped->hEvent = NULL;	// 
    pMyoverlapped->operate_type_ = OP_READ ; // 读操作
    pMyoverlapped->wsaBuf_.buf = pMyoverlapped->recv_buff_;
    pMyoverlapped->wsaBuf_.len = sizeof(pMyoverlapped->recv_buff_);

    int nResult = WSARecv (pConn->get_socket(),  // 已经和IOCP关联的socket
        &pMyoverlapped->wsaBuf_,			// 接收数据的WSABUF结构
        1,
        &transfer_bytes_, // 如立刻完成，则返回接收得到的字节数
        &dwFlag,
        (OVERLAPPED*)pMyoverlapped,  //在这里传入OVERLADDED结构
        NULL);
    if (SOCKET_ERROR  == nResult) 
    {
        if (WSAGetLastError() ==  WSA_IO_PENDING) 
        {
        }
        else 
        {
            net_overlapped_pool_.Free(pMyoverlapped);

            _ASSERT(net_event_handler_);
            check_and_disconnect(pConn);
            return false;
        }
    }

    pConn->inc_post_read_count();
    return true;
}

bool inetwork_imp::post_placement_write(net_conn* pconn, char* buff, size_t len) 	///< 投递写请求
{
    // 下面提交发送请求
    DWORD dwWriteLen = 0;
    net_overLapped *pmyoverlapped = get_net_overlapped();
    if(pmyoverlapped == NULL) 
    {
        return false;
    }

    pmyoverlapped->operate_type_ = OP_WRITE;  // 设置I/O操作类型
    pmyoverlapped->wsaBuf_.buf = buff;
    pmyoverlapped->wsaBuf_.len = len;

    int nResult = WSASend(                  // 开始发送
        pconn->get_socket(),                // 已连接的socket
        &pmyoverlapped->wsaBuf_,            // 发送buf和数据长度
        1,
        &dwWriteLen,                        // 如立刻完成，则返回发送长度
        0,	                                // 
        (LPWSAOVERLAPPED)pmyoverlapped,     // OVERLAPPED 结构
        0);                                 // 例程，不用

    if (nResult == SOCKET_ERROR)
    {
        int iErrorCode = WSAGetLastError();

        if (iErrorCode ==  WSA_IO_PENDING)
        {
            //TRACE(TEXT("iErrorCode ==  WSA_IO_PENDING\n"));
        }
        else if (iErrorCode == WSAEWOULDBLOCK)
        {
            ///TRACE(TEXT("iErrorCode == WSAEWOULDBLOCK\n"));
        }
        else 
        {
            return false;
        }
    }
    else 
    {
        //TRACE(TEXT("发送完成了"));
    }

    pconn->inc_post_write_count();
    return true;
}

//----------------------------------------------------------------------
// 
void inetwork_imp::on_read(net_conn* pConn, const char* buff, DWORD dwByteTransfered)
{
     pConn->dec_post_read_count();

    if (dwByteTransfered == 0) 
    {
        // _ASSERT(FALSE && "这里出现是对方直接关闭了连接");
        this->check_and_disconnect(pConn);
    }
    else 
    {
        // 写入到流中
        pConn->write_recv_stream(buff, dwByteTransfered);

        // 统计该套接字的读字节数
        pConn->add_readed_bytes(dwByteTransfered);

        // 通知上层处理读事件
        net_event_handler_->on_read(pConn, buff, dwByteTransfered);

        // 继续投递读操作
        this->post_read(pConn);
    }

    // 处理完了再更新活跃时间，以便可以检查连接发送数据的速度
    pConn->upsate_last_active_tm();
}

void inetwork_imp::on_write(net_conn* pconn, DWORD dwByteTransfered)
{
    // 标记已写了的数据
    pconn->mark_send_stream(dwByteTransfered);

    // 减少该套接字上的写投递计数
    pconn->dec_post_write_count();

    // 统计该套接字的写字节数
    pconn->add_writed_bytes(dwByteTransfered);

    // 通知上层处理写事件
    net_event_handler_->on_write(pconn, dwByteTransfered);

    // 更新上次活跃的时间戳
    pconn->upsate_last_active_tm();

    // 接着发送没有发送完的数据
    if (pconn->get_send_length() > 0) 
    {
        // 再发送流中的数据
        auto seg_ptr = pconn->get_send_stream().get_read_seg_ptr();
        auto seg_len = pconn->get_send_stream().get_read_seg_len();

        this->post_placement_write(pconn, (char*)seg_ptr, seg_len);
    }
}

void inetwork_imp::on_accept(net_conn* listen_conn, net_conn* accept_conn)
{
    // 设置套接字更新上下文（以便可以通过getpeername获取到ip地址）
    setsockopt(accept_conn->get_socket(), 
        SOL_SOCKET, 
        SO_UPDATE_ACCEPT_CONTEXT,  
        (char*)&( listen_conn->get_socket() ), 
        sizeof( listen_conn->get_socket()) );

    // 获取对端ip的信息
    accept_conn->init_peer_info();

    // 再次投递新的接收连接请求
    this->post_accept(listen_conn);

    // 交给上层处理事件,即INetClientImp
    net_event_handler_->on_accept(listen_conn, accept_conn); 

    // 更新时间戳
    listen_conn->upsate_last_active_tm();
    accept_conn->upsate_last_active_tm();

    //投递读请求，如果断开连接内部会处理
    this->post_read(accept_conn);
}

void inetwork_imp::on_connect(net_conn* pConn, bool bsuccess)
{
    // 连接成功，通知上层处理事件
    net_event_handler_->on_connect(pConn, true);

    // 更新上次活跃的时间戳
    pConn->upsate_last_active_tm();

    // 投递读请求
    this->post_read(pConn);
}

net_conn* inetwork_imp::create_listen_conn(USHORT uLocalPort)
{
    net_conn* pListenConn = net_conn_pool_.Alloc();
    if (NULL == pListenConn) {
        return NULL;
    }

    SOCKET& sockListen = pListenConn->get_socket();

    pListenConn->set_local_port(uLocalPort);
    pListenConn->dis_connect();
    sockListen = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
    if (INVALID_SOCKET == sockListen) {
        net_conn_pool_.Free(pListenConn);
        return NULL;
    }

    sockaddr_in local_addr;
    ZeroMemory(&local_addr, sizeof(sockaddr_in));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(pListenConn->get_local_port());
    int irt = ::bind(sockListen, (sockaddr*)(&local_addr), sizeof(sockaddr_in));
    listen(sockListen, 5);

    if (SOCKET_ERROR == irt) 
    {
        net_conn_pool_.Free(pListenConn);
        return NULL;
    }

    HANDLE hResult = CreateIoCompletionPort((HANDLE)sockListen,hiocp_, (ULONG_PTR)pListenConn, 0);
    if (NULL == hResult) 
    {
        _ASSERT(FALSE);
        net_conn_pool_.Free(pListenConn);
        return NULL;
    }

    return pListenConn;
}

bool inetwork_imp::init_threads(size_t thread_num)                 ///< 启动网络层-》创建线程
{
    thread_num_ = thread_num;

    work_threads_.resize(thread_num_);
    for (auto itr = work_threads_.begin(); itr != work_threads_.end(); ++ itr)
    {
        itr->start(std::bind(&inetwork_imp::worker_thread_, this, std::placeholders::_1), 0);
    }
    return true;
}

bool inetwork_imp::init_workers(size_t worker_num)
{
    return worker_mgr_.init(worker_num);
}

network_worker* inetwork_imp::get_worker(void* p)
{
    return worker_mgr_.choose_worker(p);
}

void inetwork_imp::worker_thread_(void* param)
{
    //使用完成端口模型
    net_overLapped *lpOverlapped = NULL;
    DWORD		    dwByteTransfered = 0;
    net_conn*       pConn = NULL;

    while (!this->m_bstop)
    {
        lpOverlapped = NULL;

        // 下面的函数调用就是去I/O出口那里等待，并获得I/O操作结果
        BOOL bResult = GetQueuedCompletionStatus(
            this->hiocp_,                       // 指定从哪个IOCP那里或地数据
            &dwByteTransfered,                  // 或得或是发送了多少字节的数据
            (PULONG_PTR)&pConn,                 // socket关联到IOCP时指定的一个关联值
            (LPWSAOVERLAPPED*)&lpOverlapped,    // 或得ConnectEx 传进来的结构
            INFINITE);				            // 一直等待，直到有结果

        _ASSERT(lpOverlapped);
        if (lpOverlapped == NULL)  // 退出
            return ;

        worker_context* ctx = g_worker_contex_pool_.Alloc();
        if (NULL == ctx) {
            return;
        }

        ctx->network_                     = this;
        ctx->pConn                        = pConn;
        ctx->ctx_type_                    = context_recv_ctx;

        ctx->recv_info.bResult            = bResult;
        ctx->recv_info.dwByteTransfered   = dwByteTransfered;
        ctx->recv_info.lpOverlapped       = lpOverlapped;
        get_worker(pConn)->send(ctx);        
    }

    return;
}

net_overLapped* inetwork_imp::get_net_overlapped()
{
    net_overLapped* ptmp = net_overlapped_pool_.Alloc();
    if (ptmp) 
    {
        memset(ptmp, 0, sizeof(OVERLAPPED));
    }

    return ptmp;
}

void inetwork_imp::check_and_disconnect(net_conn* pConn)
{
    if (!pConn->get_is_closing() && 
        pConn->get_post_read_count() == 0 && 
        pConn->get_post_read_count() == 0) 
    {
        pConn->set_is_closing(TRUE);

        net_event_handler_->on_disconnect(pConn);
    }
}