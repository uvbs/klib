#pragma once
#pragma warning(disable: 4996)

#include "MySocket.h"
#include <string>
#include <list>

#include <kthread/auto_lock.h>
#include <io/mem_seg_stream.h>

using namespace klib::kthread;


#define  MAX_CLIENT_BUFF_LEN 5*1024

//------------------------------------------------------------------------
// 操作类型
enum emOperationType
{
    OP_NONE,
    OP_ACCEPT,
    OP_READ,
    OP_WRITE,
    OP_CONNECT,
};

typedef klib::io::mem_seg_stream<2048> net_stream_type;

class NetPacket;
class net_conn
{
public:
    net_conn(void);
    ~net_conn(void);

public:
    inline NetSocket& get_socket() { return m_socket; }
    bool init_peer_info();  // 初始对端信息，通过getpeername来获取
    inline void set_peer_addr(const char* straddr) { strncpy(m_strAddress, straddr, sizeof(m_strAddress)-1); }
    char* get_peer_addr() ;

    inline void set_peer_port(USHORT port) { peer_port_ = port; }
    inline USHORT get_peer_port() { return peer_port_; }
    inline void set_local_port(USHORT port) { local_port_ = port; }
    inline USHORT get_local_port() { return local_port_; }

    inline void dis_connect() {  closesocket(m_socket); m_socket = INVALID_SOCKET; }

    const net_stream_type& get_recv_stream() { return recv_stream_; }
    bool write_recv_stream(const char* buff, size_t len) ;
    bool read_recv_stream(char* buff, size_t len);
    size_t get_recv_length();

    inline DWORD get_last_active_tm() { return last_active_tm_; }
    void upsate_last_active_tm() ;

    void inc_post_read_count();
    void dec_post_read_count();
    USHORT get_post_read_count();
    void inc_post_write_count();
    void dec_post_write_count();
    USHORT get_post_write_count();

    inline BOOL get_is_closing() { return m_bClosing; }
    inline void set_is_closing(BOOL bClose = TRUE) { m_bClosing = bClose; }

    inline void set_bind_key(void* key) { bind_key_ = key; }
    inline void* get_bind_key() { return bind_key_; }

    inline DWORD get_writed_bytes() { return m_dwBytesWrite; }
    inline DWORD add_rwited_bytes(DWORD dwBytes) { return (m_dwBytesWrite+=dwBytes); }
    inline DWORD get_readed_bytes() { return m_dwBytesRead; }
    inline DWORD add_readed_bytes(DWORD dwBytes) { return m_dwBytesRead += dwBytes; }

    void lock() { mutex_.lock(); }
    void unlock(){ mutex_.unlock() ;}

protected:
    NetSocket m_socket;		//套接字
    DWORD     last_active_tm_;	//最后活跃时间

    USHORT    peer_port_;		//对端端口,主机序
    DWORD     peer_addr_;		//对端地址，网络字节序

    USHORT    local_port_;            //本地端口，做监听用的
    USHORT    post_read_count_;        //投递接收的数量
    USHORT    post_write_count_;       //投递发送的数量
    BOOL      m_bClosing;             //指示是否是在关闭
    void*     bind_key_;               //绑定的键值
    DWORD     m_dwBytesWrite;         //输出了多少字节数
    DWORD     m_dwBytesRead;          //接收到了多少字节数
    mutex     mutex_;                 //临界区对象，用于互斥数据的访问
    char      m_strAddress[50];		//字符串地址
    int       datalen;					    //缓冲区中保存数据的长度

    net_stream_type  recv_stream_;
    net_stream_type  send_stream_;
};