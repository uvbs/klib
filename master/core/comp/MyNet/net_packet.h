#pragma once

// 保存网络数据包的
class net_conn;
class tcp_net_facade_imp;
class inetpacket_mgr_imp;

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
    bool        is_fixed_mem_;                  // 表示是否是固定的，即不需要被delete掉的
    net_conn*   pConn;	                        // 属于哪个连接

    size_t      bf_size_;                       // 缓存区中保存数据的大小
    char*       bf_ptr_;                        // 缓冲区
};