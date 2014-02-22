#pragma once

class net_conn;
class tcp_net_facade_imp;
class inetpacket_mgr_imp;

// 保存网络数据包的
class net_packet
{
    friend tcp_net_facade_imp;
    friend inetpacket_mgr_imp;
public:
    net_packet(void);
    ~net_packet(void);

    bool init_buff(size_t bf_size);
    char* get_buff() ;

protected:
    net_conn*   pConn;	                        // 属于哪个连接

    size_t      bf_size_;                       // 缓存区中保存数据的大小
    char*       bf_ptr_;                        // 缓冲区
};