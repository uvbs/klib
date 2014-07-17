#ifndef _klib_network_imp_h
#define _klib_network_imp_h

#include "network_i.h"
#include "net_socket.h"
#include "net_conn.h"
#include "net_conn_mgr_i.h"

#include <list>
#include <vector>
#include <Mswsock.h>

#include <kthread/thread.h>
#include <kthread/thread_local.h>

#include <pattern/object_pool.h>
#include <pattern/active_object.h>

using namespace klib::kthread;
using namespace klib::pattern;


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

typedef std::vector<Thread> thread_vec_type;

#define  RECV_BUFF_LEN   (2*1024)
//----------------------------------------------------------------------
// 单句柄数据
//  处理网络相关数据
class net_overLapped: public OVERLAPPED
{
public:
    //外部使用
    em_operation_type   operate_type_;              ///< 提交的操作类型
    net_conn_ptr        op_conn_;                   ///< 连接对象
    net_conn_ptr        listen_conn_;               ///< 监听的连接，有可能为空
    DWORD               transfer_bytes_;            ///< 传输了的数据
    WSABUF              wsaBuf_;                    ///< 缓冲区对象
    char                recv_buff_[RECV_BUFF_LEN];		///< 接收数据的buf
    
public:
    net_overLapped() : /*pend_data_(0),*/ 
        operate_type_(OP_NONE),
        transfer_bytes_(0)
    {
    }
};

enum contex_type
{
    context_recv_ctx,
    context_send_ctx,
};

class network_imp;
class worker_context
{
public:
    contex_type                 ctx_type_;
    network_imp*                network_;
    std::weak_ptr<net_conn>     pConn;
    union
    {
        struct recv_info
        {
            net_overLapped*     lpOverlapped;
            DWORD               dwByteTransfered;
            BOOL                bResult;
        } recv_info;

        struct send_info
        {
            char*               buff_ptr_;
            size_t              buff_len_;
        } send_info;
    };
};

class network_worker : public active_object<worker_context*>
{
public:
    virtual bool execute(worker_context*& ctx);

protected:
    bool on_recv_ctx(worker_context*& ctx);
    bool on_send_ctx(worker_context*& ctx);
};


/// 网络库实现
class network_imp : public network_i
{
    friend network_worker;

public:
    network_imp(void);
    ~network_imp(void);

public:
    //----------------------------------------------------------------------
    // 接口实现
    virtual bool init_network(inet_tcp_handler* handler, 
        size_t thread_num = 1,
        size_t worker_num = 7) ;

    virtual net_conn_mgr_i* get_net_conn_mgr() { return net_conn_mgr_i_.get(); }

    virtual bool try_write(net_conn_ptr pconn, const char* buff, size_t len);          ///< 尝试发送数据

    virtual net_conn_ptr try_listen(USHORT local_port, void* bind_key = NULL) ;                                  ///< 监听端口
    virtual net_conn_ptr try_connect(const char* addr, USHORT uport, void* bind_key) ;    ///< 投递连接到服务器

protected:
    //----------------------------------------------------------------------
    // 内部异步投递函数
    virtual net_conn_ptr post_accept(net_conn_ptr pListenConn) ;	                        ///< 投递接受请求,可多次投递，监听套接字必须用 create_listen_conn 创建
    virtual bool post_connection(net_conn_ptr pConn) ;		                            ///< 投递连接请求
    virtual bool post_read(net_conn_ptr pConn) ;	                                    ///< 投递读请求，在接受连接后底层会默认投递一个请求
    //virtual bool post_write(net_conn_ptr pConn, char* buff, size_t len) ;	        ///< 投递写请求
    virtual bool post_placement_write(net_conn_ptr pConn, char* buff, size_t len) ;	///< 投递写请求

protected:
    //----------------------------------------------------------------------
    // 处理完成事件
    void on_read(net_conn_ptr, const char* buff, DWORD dwtransfer);
    void on_write(net_conn_ptr, DWORD dwByteTransfered);
    void on_accept(net_conn_ptr listen_conn, net_conn_ptr accept_conn);
    void on_connect(net_conn_ptr, bool bsuccess);

protected:
    //----------------------------------------------------------------------
    // net_conn 资源管理 
    virtual net_conn_ptr create_listen_conn(USHORT uLocalPort) ;	            ///< 创建返回监听套接字，返回一个net_conn结构，用于接受连接
    net_overLapped* get_net_overlapped();                                   ///< 申请重叠结构

    //----------------------------------------------------------------------
    // 定时器相关

    //----------------------------------------------------------------------
    // 初始化
    bool init_threads(size_t thread_num) ;                                  ///< 启动网络层-》创建线程
    bool init_workers(size_t worker_num);                                   ///< 初始化工作对象
    network_worker* get_worker(void*);                                     ///< 获取worker

protected:
    void worker_thread_();                                       ///< 工作线程
    void check_and_disconnect(net_conn_ptr pConn);                             ///< 判断在套接字上还有没有未处理的投递请求，如果没有了则断开连接

private:
    // handle
    HANDLE                  hiocp_;                                         ///< 完成端口句柄
    inet_tcp_handler*       net_event_handler_;                             ///< 移交上层处理的接口
    LPFN_ACCEPTEX           m_lpfnAcceptEx;                                 ///< AcceptEx函数指针
    LPFN_CONNECTEX          m_lpfnConnectEx;
    bool                    m_bstop;

    thread_vec_type         work_threads_;
    size_t                  thread_num_;

    active_obj_mgr<network_worker> worker_mgr_;

    CObjectPool<net_overLapped, 2000, 2000>     net_overlapped_pool_;       // overlapped 
    
    std::shared_ptr<net_conn_mgr_i> net_conn_mgr_i_;
};

#endif
