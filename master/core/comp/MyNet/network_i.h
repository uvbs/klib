#ifndef _klib_network_i_h
#define _klib_network_i_h

#include "net_conn.h"
#include "net_socket.h"

class inet_tcp_handler;
class net_conn_mgr_i;

///< 网络接口类
class network_i
{
public:
    network_i(void) {}
    virtual ~network_i(void) {}

public:
    ///< 初始化网络层接口，由tcp_net_facade调用
    virtual bool init_network(inet_tcp_handler* handler, 
        size_t thread_num = 1,
        size_t worker_num = 6) = 0; 

    virtual net_conn_mgr_i* get_net_conn_mgr() = 0;

    ///< 投递一个写请求
    virtual bool try_write(net_conn_ptr pconn, const char* buff, size_t len) = 0;

    ///< 创建获取监听套接口
    virtual net_conn_ptr try_listen(USHORT uLocalPort, void* bind_key = NULL) = 0;
    
    ///< 投递连接到服务器
    virtual net_conn_ptr try_connect(const char* addr, USHORT uport, void* bind_key = NULL) = 0;
};

#endif
