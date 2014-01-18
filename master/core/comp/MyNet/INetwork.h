#pragma once

#include "net_socket.h"

class net_conn;
class inet_tcp_handler;

///< 网络接口类
class inetwork
{
public:
    inetwork(void) {}
    virtual ~inetwork(void) {}

public:
    ///< 初始化网络层接口，由tcp_net_facade调用
    virtual bool init_network(inet_tcp_handler* handler) = 0; 

    ///< 运行网络层，创建线程这些
    virtual bool run_network() = 0;

    ///< 投递接受连接
    /// @usage   net_conn* pListenConn = pClient->get_network()->CreateNewConnection();
    /// pListenConn->set_local_port(7000);
    /// pClient->get_network()->InitListenConnection(pListenConn);
    /// pClient->get_network()->post_accept(pListenConn);
    virtual net_conn* post_accept(net_conn* plistenConn) = 0;
    
    ///< 在建立的连接上投递一个读请求
    /// @usage   net_conn* pMyConn = pClient->get_network()->CreateNewConnection();
    /// pClient->get_network()->post_read(pMyConn);
    virtual bool post_read(net_conn* pConn) = 0;

    ///< 在建立的连接上投递一个写数据请求
    /// @usage   net_conn* pMyConn = pClient->get_network()->CreateNewConnection();
    /// pClient->get_network()->post_write(pMyConn, buff, 1024);
    virtual bool post_write(net_conn* pConn, const char* buff, size_t len) = 0;

    ///< 创建获取监听套接口
    virtual net_conn* try_listen(USHORT uLocalPort) = 0;
    
    ///< 投递连接到服务器
    virtual net_conn* try_connect(const char* addr, USHORT uport, void* bind_key = NULL) = 0;
};