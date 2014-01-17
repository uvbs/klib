#pragma once

#include "net_socket.h"

class net_conn;
class inet_event_handler;
class inetwork
{
public:
    inetwork(void) {}
    virtual ~inetwork(void) {}

public:
    /// 
    /// @brief 初始化网络层接口，由tcp_net_facade调用
    virtual bool init_network(inet_event_handler* handler) = 0; 

    /// @brief 运行网络层，创建线程这些
    virtual bool run_network() = 0;

    /// @brief 投递接受连接
    ///
    /// @param[in] plistenConn 监听套接字接口，需要初始化 调用对像的@ref InitListenConnection方法
    /// @return 新创建的用于接受客户端连接的套接字接口
    /// @retval NULL 投递失败
    /// @retval !=NULL  新创建的用于接收连接的套接字接口,类型是 net_conn*
    /// @post  创建套接字接口，并投递一个AcceptEx请求，当系统处理完毕时会收到一个完成请求
    ///
    /// @usage   net_conn* pListenConn = pClient->get_network()->CreateNewConnection();
    /// pListenConn->set_local_port(7000);
    /// pClient->get_network()->InitListenConnection(pListenConn);
    /// pClient->get_network()->post_accept(pListenConn);
    ///
    virtual net_conn* post_accept(net_conn* plistenConn) = 0;

    /// @brief 投递连接
    /// 
    /// 连接对方服务器ip+端口
    ///
    /// @param[in] pConn  创建的连接接口
    /// @return 表示投递是否成功，并不表示是否已连接
    /// @retval false 投递失败
    /// @retval true  投递成功
    /// @post  投递连接请求到完成端口上
    ///
    /// @usage   net_conn* pMyConn = pClient->get_network()->CreateNewConnection();
    /// pMyConn->set_peer_addr("127.0.0.1");
    /// pMyConn->set_peer_port(7000);
    /// pClient->get_network()->post_connection(pMyConn);
    /// 
    virtual bool post_connection(net_conn* pConn) = 0;

    /// @brief 在建立的连接上投递一个读请求
    ///
    /// @param[in] pConn  已经连接的连接接口
    /// @return 表示投递是否成功，并不表示是否已读取到数据
    /// @retval false 投递失败
    /// @retval true  投递成功
    /// @post  
    ///
    /// @usage   net_conn* pMyConn = pClient->get_network()->CreateNewConnection();
    /// pClient->get_network()->post_read(pMyConn);
    /// 
    virtual bool post_read(net_conn* pConn) = 0;

    /// @brief 在建立的连接上投递一个写数据请求
    ///
    /// @param[in] pConn  已经连接的连接接口
    /// @return 表示投递是否成功，并不表示是否已发送完数据
    /// @retval false 投递失败
    /// @retval true  投递成功
    /// @post  
    ///
    /// @usage   net_conn* pMyConn = pClient->get_network()->CreateNewConnection();
    /// pClient->get_network()->post_write(pMyConn, buff, 1024);
    /// 
    virtual bool post_write(net_conn* pConn, const char* buff, size_t len) = 0;

    /// @brief 创建获取监听套接口
    ///
    /// @param[in] plistenConn  使用@ref CreateNewConnection方法创建的接口
    /// @return net_conn 创建的套接字接口
    /// @retval NULL   创建失败
    /// @retval != NULL  创建成功
    /// @post  
    ///
    /// @usage   net_conn* pListenConn = create_listen_conn(9000);
    ///          使用post_accept(pListenConn); 投递接受请求
    virtual net_conn* create_listen_conn(USHORT uLocalPort) = 0;

    /// @brief 获取一个套接字接口
    ///
    /// 从链表是获取存在的套接字接口，如果没有的话就会new生成一个并返回
    ///
    /// @return 套接字接口
    /// @retval NULL   创建失败
    /// @retval !=NULL  创建成功
    /// @post  
    ///
    /// @usage   net_conn* pListenConn = pClient->get_network()->CreateNewConnection();
    /// 
    virtual net_conn* create_conn() = 0;

    /// @brief 销毁一个套接字接口
    ///
    /// 如果链表中的数量比较大的话，该占用内存就会被释放，如果链表中比较小则不会释放
    ///
    /// @param[in] pConn   创建成功了的套接字接口
    /// @return 表示是否释放成功
    /// @retval false   释放失败
    /// @retval true    释放成功
    /// @post  
    ///
    /// @usage   net_conn* pListenConn = pClient->get_network()->release_conn(pConn);
    /// 
    virtual bool release_conn(net_conn* pConn) = 0;
};