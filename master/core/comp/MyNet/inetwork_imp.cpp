#include "StdAfx.h"
#include "INetworkImp.h"
#include <process.h>

#include "inet_event_handler.h"
#include "net_conn.h"
#include <net/addr_resolver.h>

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

inetwork_imp::inetwork_imp(void)
{
    m_lpfnAcceptEx = NULL;
    hiocp_ = INVALID_HANDLE_VALUE;

    //初始链表
    init_fixed_overlapped_list(100);
}

inetwork_imp::~inetwork_imp(void)
{
}

bool inetwork_imp::init_network()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    hiocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,(ULONG_PTR)0, 0);
    _ASSERT(hiocp_);
    if (NULL == hiocp_) {
        return false;
    }

    //TRACE(TEXT("初始网络成功"));

    return true;
}

bool inetwork_imp::set_net_event_handler(inet_event_handler* handler)
{
    _ASSERT(handler);
    net_event_handler_ = handler;
    return true;
}

bool inetwork_imp::run_network() 
{
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &inetwork_imp::work_thread_, this, 0, NULL);
    CloseHandle(hThread);

    return true;
}

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
        release_net_overlapped(lpOverlapped);
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
            release_net_overlapped(lpOverlapped);
            //TRACE(TEXT("fail at WSAIoctl-AcceptEx,Error Code:%d\n"));
            return NULL;
        }
    }

    net_conn* pNewConn = create_conn();
    if (NULL == pNewConn) {
        closesocket(sockAccept);
        release_net_overlapped(lpOverlapped);
        return NULL;
    }

    hResult = CreateIoCompletionPort((HANDLE)sockAccept, hiocp_, (ULONG_PTR)pNewConn, 0);
    if (NULL == hResult) {
        closesocket(sockAccept);
        release_net_overlapped(lpOverlapped);
        release_conn(pNewConn);
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
        lpOverlapped->buff_, 
        0,//lpToverlapped->dwBufSize,
        sizeof(sockaddr_in) + 16, 
        sizeof(sockaddr_in) + 16, 
        &(lpOverlapped->transfer_bytes_), 
        (LPOVERLAPPED)lpOverlapped);


    if(brt == FALSE && WSA_IO_PENDING != WSAGetLastError()) {
        closesocket(sockAccept);
        release_net_overlapped(lpOverlapped);
        release_conn(pNewConn);

        DWORD dwErrorCode = WSAGetLastError();
        //TRACE(TEXT("fail at lpfnAcceptEx,error code:%d\n"), dwErrorCode);
        return NULL;
    }
    else if (WSA_IO_PENDING == WSAGetLastError()) {
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
    klib::net::addr_resolver ipresover(pConn->get_peer_addr());
    if (ipresover.size() < 0)  {
        return false;
    }
    uPeerAddr = ipresover.at(0);

    SOCKET& sock = pConn->get_socket();
    sock = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
    if (sock == INVALID_SOCKET) {
        _ASSERT(FALSE);  // DWORD dwErr = WSAGetLastError();
        _ASSERT(!"创建套接字失败!!!");
        return false;
    }

    sockaddr_in local_addr;
    ZeroMemory(&local_addr, sizeof(sockaddr_in));
    local_addr.sin_family = AF_INET;
    int iret = ::bind(sock, (sockaddr*)(&local_addr), sizeof(sockaddr_in));
    if (SOCKET_ERROR == iret) {
        closesocket(sock);
        _ASSERT(!"绑定端口失败!!!");
        return false;
    }

    HANDLE hResult = CreateIoCompletionPort((HANDLE)sock,hiocp_, (ULONG_PTR)pConn,0);
    if (NULL == hResult) {
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
    if (NULL == pmyoverlapped) {
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
        if( WSAGetLastError() != ERROR_IO_PENDING) { // 调用失败
            closesocket(sock);
            _ASSERT(FALSE);
            //TRACE(TEXT("ConnextEx error: %d\n"));
            return FALSE;
        }
        else {//操作未决（正在进行中…）
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
    pMyoverlapped->wsaBuf_.buf = pMyoverlapped->buff_;
    pMyoverlapped->wsaBuf_.len = sizeof(pMyoverlapped->buff_);

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
            release_net_overlapped(pMyoverlapped);

            _ASSERT(net_event_handler_);
            check_and_disconnect(pConn);
            return false;
        }
    }

    pConn->inc_post_read_count();
    return true;
}

bool inetwork_imp::post_write(net_conn* pConn, const char* buff, size_t len) 
{
    // 下面提交发送请求
    DWORD dwWriteLen = 0;
    net_overLapped *pmyoverlapped = get_net_overlapped();
    if(pmyoverlapped == NULL) {
        return false;
    }

    memcpy(pmyoverlapped->buff_, buff, len);  // 填数据到buf中
    pmyoverlapped->operate_type_ = OP_WRITE;  //设置I/O操作类型
    pmyoverlapped->wsaBuf_.buf = pmyoverlapped->buff_;
    pmyoverlapped->wsaBuf_.len = len;

    int nResult =  WSASend(  //开始发送
        pConn->get_socket(),  // 已连接的socket
        &pmyoverlapped->wsaBuf_, // 发送buf和数据长度
        1,
        &dwWriteLen,// 如立刻完成，则返回发送长度
        0,	// 
        (LPWSAOVERLAPPED)pmyoverlapped, // OVERLAPPED 结构
        0); // 例程，不用

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
        else {
            return false;
        }
    }
    else {
        //TRACE(TEXT("发送完成了"));
    }

    pConn->inc_post_write_count();
    return true;
}

net_conn* inetwork_imp::create_listen_conn(USHORT uLocalPort)
{
    net_conn* pListenConn = create_conn();
    if (NULL == pListenConn) {
        return NULL;
    }

    SOCKET& sockListen = pListenConn->get_socket();

    pListenConn->set_local_port(uLocalPort);
    pListenConn->dis_connect();
    sockListen = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

    sockaddr_in local_addr;
    ZeroMemory(&local_addr, sizeof(sockaddr_in));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(pListenConn->get_local_port());
    int irt = ::bind(sockListen, (sockaddr*)(&local_addr), sizeof(sockaddr_in));
    listen(sockListen, 5);

    if (SOCKET_ERROR == irt) {
        release_conn(pListenConn);
        return NULL;
    }

    HANDLE hResult = CreateIoCompletionPort((HANDLE)sockListen,hiocp_, (ULONG_PTR)pListenConn, 0);
    if (NULL == hResult) {
        _ASSERT(FALSE);
        release_conn(pListenConn);
        return NULL;
    }

    return pListenConn;
}

net_conn* inetwork_imp::create_conn() 
{
    net_conn* pConn = NULL;
    auto_lock helper(free_net_conn_mutex_);
    if (!free_net_conn_list_.empty()) {
        pConn = free_net_conn_list_.front();
        new (pConn) net_conn;   //placement new
        free_net_conn_list_.pop_front();
    }
    else {
        pConn = new net_conn;
    }
    return pConn;
}

bool inetwork_imp::release_conn(net_conn* pConn)
{
    _ASSERT(pConn);
    auto_lock helper(free_net_conn_mutex_);

#ifdef _DEBUG

    INetConnListType::const_iterator itr;
    itr = free_net_conn_list_.begin();
    for (; itr != free_net_conn_list_.end(); ++itr) {
        if ((*itr) == pConn) {
            _ASSERT(FALSE && "设计有问题，请检查设计!");
        }
    }
#endif

    if (free_net_conn_list_.size() > 1000) {
        delete pConn;
    }
    else {
        pConn->~net_conn();
        free_net_conn_list_.push_back(pConn);
    }

    return true;
}

unsigned int WINAPI inetwork_imp::work_thread_(void* param)
{
    //使用完成端口模型
    inetwork_imp* pINetwork = (inetwork_imp*)param;
    inet_event_handler* pINetEventHandler = pINetwork->net_event_handler_;
    _ASSERT(pINetwork);
    _ASSERT(pINetwork->hiocp_);
    _ASSERT(pINetEventHandler);
    net_overLapped *lpOverlapped = NULL;
    DWORD		dwByteTransfered = 0;
    net_conn *pConn = NULL;

    while (true)
    {
        lpOverlapped = NULL;

        // 下面的函数调用就是去I/O出口那里等待，并获得I/O操作结果
        BOOL bResult = GetQueuedCompletionStatus(
            pINetwork->hiocp_, // 指定从哪个IOCP那里或地数据
            &dwByteTransfered, // 或得或是发送了多少字节的数据
            (PULONG_PTR)&pConn, // socket关联到IOCP时指定的一个关联值
            (LPWSAOVERLAPPED*)&lpOverlapped,  // 或得ConnectEx 传进来的结构
            INFINITE);				// 一直等待，直到有结果

        _ASSERT(lpOverlapped);
        if (lpOverlapped == NULL)  {
            //TRACE(TEXT("退出...."));
            return 0;
        }

        if (bResult) {
            if(dwByteTransfered==-1 && lpOverlapped==NULL) {
                //TRACE(TEXT("退出线程并结束..."));
                return 1L;
            }

            switch(lpOverlapped->operate_type_ )
            {
            case OP_READ:
                {
                    pConn->dec_post_read_count();
                    if (dwByteTransfered == 0) {

                        //_ASSERT(FALSE && "这里出现是对方直接关闭了连接");
                        pINetwork->check_and_disconnect(pConn);
                    }
                    else {
                        //统计该套接字的读字节数
                        pConn->add_readed_bytes(dwByteTransfered);

                        //通知上层处理读事件
                        pINetEventHandler->on_read(pConn, (const char*)lpOverlapped->buff_, dwByteTransfered);

                        //继续投递读操作
                        pINetwork->post_read(pConn);
                    }
                    //处理完了再更新活跃时间，以便可以检查连接发送数据的速度
                    pConn->upsate_last_active_tm();
                }
                break;

            case OP_WRITE:
                {
                    //减少该套接字上的写投递计数
                    pConn->dec_post_write_count();

                    //统计该套接字的写字节数
                    pConn->add_rwited_bytes(dwByteTransfered);

                    //通知上层处理写事件
                    pINetEventHandler->on_write(pConn);

                    //更新上次活跃的时间戳
                    pConn->upsate_last_active_tm();
                }
                break;
            case OP_ACCEPT:
                {
                    net_conn* pListConn = pConn;      //监听套接字
                    net_conn* pAcceptConn = (net_conn*) lpOverlapped->pend_data_;    //接收的连接

                    //设置套接字更新上下文（以便可以通过getpeername获取到ip地址）
                    setsockopt(pAcceptConn->get_socket(), 
                        SOL_SOCKET, 
                        SO_UPDATE_ACCEPT_CONTEXT,  
                        ( char* )&( pListConn->get_socket() ), 
                        sizeof( pListConn->get_socket()) );

                    //获取对端ip的信息
                    pAcceptConn->init_peer_info();

                    //再次投递新的接收连接请求
                    pINetwork->post_accept(pListConn);

                    //交给上层处理事件,即INetClientImp
                    pINetEventHandler->on_accept(pListConn, pAcceptConn); 

                    //更新时间戳
                    pListConn->upsate_last_active_tm();
                    pAcceptConn->upsate_last_active_tm();

                    //投递读请求，如果断开连接内部会处理
                    pINetwork->post_read(pAcceptConn);
                }
                break;

            case OP_CONNECT:
                {
                    //TRACE(TEXT("connected successfully!\n"));

                    //连接成功，通知上层处理事件
                    pINetEventHandler->on_connect(pConn, true);

                    //更新上次活跃的时间戳
                    pConn->upsate_last_active_tm();

                    //投递读请求
                    pINetwork->post_read(pConn);
                }
                break;
            }

            //释放lpOverlapped结构，每次有请求的时候重新获取
            pINetwork->release_net_overlapped(lpOverlapped);
        }
        else {
            //@todo 需要处理断开连接的处理

            if (lpOverlapped->operate_type_ == OP_READ) {
                pConn->dec_post_read_count();
            }
            else if (lpOverlapped->operate_type_ == OP_WRITE) {
                pConn->dec_post_write_count();
            }

            if (lpOverlapped->operate_type_ == OP_CONNECT) {
                pConn->dis_connect();
                pINetEventHandler->on_connect(pConn, false);
            }
            else if (lpOverlapped->operate_type_ == OP_ACCEPT) {
                //
                net_conn* pListConn = pConn;      //监听套接字
                net_conn* pAcceptConn = (net_conn*) lpOverlapped->pend_data_;    //接收的连接
                pINetwork->post_accept(pListConn);
                pINetEventHandler->on_accept(pListConn, pAcceptConn, false);

                pINetwork->release_conn(pAcceptConn);
            }
            else {
                //处理其它操作（如需要释放连接）
                pConn->dis_connect();
                pINetwork->check_and_disconnect(pConn);
                pINetwork->release_conn(pConn);
            }

            pINetwork->release_net_overlapped(lpOverlapped);
        } //if (bResult) {

    }

    return 0;
}

void inetwork_imp::init_fixed_overlapped_list(size_t nCount)
{
    net_overLapped* pList = new net_overLapped[nCount];
    if (NULL == pList) {
        return;
    }

    for (size_t i=0; i<nCount; ++i)
    {
        pList[i].bFixed = true;
        overlapped_list_.push_back(&pList[i]);
    }
}

net_overLapped* inetwork_imp::get_net_overlapped()
{
    auto_lock helper(overlapped_list_mutex_);

    net_overLapped* ptmp = NULL;
    if (!overlapped_list_.empty()) {
        ptmp = overlapped_list_.front();
        overlapped_list_.pop_front();

        bool bFix = ptmp->bFixed;
        memset(ptmp, 0, sizeof(OVERLAPPED));
        ptmp->bFixed = bFix;

        //TRACE(TEXT("中内存池中获取net_overLapped..."));
    }
    else {
        //TRACE(TEXT("新建net_overLapped结构..."));
        ptmp = new net_overLapped;
    }

    return ptmp;
}

bool inetwork_imp::release_net_overlapped(net_overLapped* pMyoverlapped)
{
    _ASSERT(pMyoverlapped);
    if (NULL == pMyoverlapped) {
        return false;
    }

    auto_lock helper(overlapped_list_mutex_);

    if (!pMyoverlapped->bFixed) {
        delete pMyoverlapped;
        //TRACE(TEXT("没保存，释放内存..."));
    }
    else {
        //TRACE(TEXT("保存Overlapped到链表中..."));
        overlapped_list_.push_back(pMyoverlapped);
    }

    return true;
}

void inetwork_imp::check_and_disconnect(net_conn* pConn)
{
    pConn->lock();
    if (!pConn->get_is_closing() && pConn->get_post_read_count() == 0 && pConn->get_post_read_count() == 0) 
    {
        pConn->set_is_closing(TRUE);
        pConn->unlock();

        net_event_handler_->on_disconnect(pConn);
    }
    else 
    {
        pConn->unlock();
    }
}