#pragma once
#pragma warning(disable: 4996)

#include "net_socket.h"
#include <string>
#include <list>

#include <kthread/auto_lock.h>
#include <io/mem_seg_stream.h>

using namespace klib::kthread;


#define  MAX_CLIENT_BUFF_LEN 5*1024

//------------------------------------------------------------------------
// 操作类型
enum em_operation_type
{
    OP_NONE,
    OP_ACCEPT,              ///< 接受连接请求
    OP_READ,                ///< 读请求
    OP_WRITE,               ///< 写请求
    OP_CONNECT,             ///< 连接请求
};

typedef klib::io::mem_seg_stream<2048> net_stream_type;

class net_packet;
class tcp_net_facade_imp;
class inetwork_imp;

//----------------------------------------------------------------------
///< 网络连接类,需要同步的在外部加锁
class net_conn
{
    friend tcp_net_facade_imp;
    friend inetwork_imp;
public:
    net_conn(void);
    ~net_conn(void);

    typedef void* bind_key_type;

public:
    // 获取相关信息
    inline net_socket& get_socket() { return m_socket; }
    bool init_peer_info();                                      // 初始对端信息，通过getpeername来获取
    void set_peer_addr_str(const char* straddr) {strncpy(peer_addr_str_, straddr, sizeof(peer_addr_str_) - 1);}                  // 设置ip地址
    char* get_peer_addr_str() ;

    inline void   set_peer_port(USHORT port) { peer_port_ = port; }
    inline USHORT get_peer_port() { return peer_port_; }
    inline void   set_local_port(USHORT port) { local_port_ = port; }
    inline USHORT get_local_port() { return local_port_; }

    // 断开连接
    void dis_connect() ;

    // 接收流的处理
    const net_stream_type& get_recv_stream() { return recv_stream_; }
    bool    read_recv_stream(char* buff, size_t len);
    size_t  get_recv_length();

    // 发送流的处理
    const net_stream_type& get_send_stream() { return send_stream_; }
    bool    write_send_stream(const char* buff, size_t len) ;
    bool    mark_send_stream(size_t len);
    size_t  get_send_length();

    // 绑定数据
    inline void  set_bind_key(void* key) { bind_key_ = key; }
    inline void* get_bind_key() { return bind_key_; }

    // 提供给外部的锁，用于相关必须要用到锁的场景
    mutex& get_lock() { return mutex_; }
    void lock() { mutex_.lock(); }
    void unlock(){ mutex_.unlock() ;}

    // 获取流量统计
    inline size_t get_writed_bytes()               { return writed_bytes_; }
    inline size_t get_readed_bytes()               { return readed_bytes_; }

protected:
    bool write_recv_stream(const char* buff, size_t len) ;
    bool read_send_stream(char* buff, size_t len);

    // 时间计数
    inline DWORD get_last_active_tm() { return last_active_tm_; }
    void upsate_last_active_tm() ;

    // 投递计数
    void inc_post_read_count();
    void dec_post_read_count();
    USHORT get_post_read_count();
    void inc_post_write_count();
    void dec_post_write_count();
    USHORT get_post_write_count();

    // 是否关闭状态
    inline bool get_is_closing() { return is_closing_; }
    inline void set_is_closing(bool bClose = TRUE) { is_closing_ = bClose; }

    // 设置流量统计
    inline size_t add_rwited_bytes(size_t dwBytes) { return (writed_bytes_+=dwBytes); }
    inline size_t add_readed_bytes(size_t dwBytes) { return readed_bytes_ += dwBytes; }

protected:
    net_socket m_socket;		            // 套接字
    DWORD     last_active_tm_;	            // 最后活跃时间

    USHORT    peer_port_;		            // 对端端口,主机序
    DWORD     peer_addr_dw_;		        // 对端地址，网络字节序
    char      peer_addr_str_[50];		    // 字符串地址

    USHORT    local_port_;                  // 本地端口，做监听用的
    USHORT    post_read_count_;             // 投递接收的数量
    USHORT    post_write_count_;            // 投递发送的数量
    bool      is_closing_;                  // 指示是否是在关闭
    void*     bind_key_;                    // 绑定的键值

    size_t    writed_bytes_;                // 输出了多少字节数
    size_t    readed_bytes_;                // 接收到了多少字节数

    mutex     mutex_;                       // 临界区对象，用于互斥数据的访问

    net_stream_type  recv_stream_;          // 接收流
    net_stream_type  send_stream_;          // 发送流
};