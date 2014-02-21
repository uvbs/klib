#pragma once

#include "INetwork.h"
#include "net_socket.h"

#include <list>
#include <vector>
#include <Mswsock.h>

#include <kthread/thread.h>
#include <kthread/thread_local.h>

#include <pattern/object_pool.h>
#include <pattern/active_object.h>

using namespace klib::kthread;
using namespace klib::pattern;

typedef std::vector<Thread> thread_vec_type;

//----------------------------------------------------------------------
// 单句柄数据
//  处理网络相关数据
class net_overLapped: public OVERLAPPED
{
public:
    //外部使用
    int       operate_type_;            ///< 提交的操作类型
    void*     pend_data_;               ///< 附带其它数据的
    DWORD     transfer_bytes_;          ///< 传输了的数据
    WSABUF    wsaBuf_;                  ///< 缓冲区对象
    char      recv_buff_[2 * 1024];		///< 接收数据的buf
    
public:
    net_overLapped() : pend_data_(0), transfer_bytes_(0)
    {
    }
};

class inetwork_imp;
class worker_context
{
public:
    inetwork_imp*       network_;
    net_overLapped*     lpOverlapped;
    DWORD		        dwByteTransfered;
    net_conn*           pConn;
    BOOL                bResult;
};

class network_worker : public active_object<worker_context*>
{
public:
     virtual bool execute(worker_context*& ctx);
};


/// 网络库实现
class inetwork_imp : public inetwork
{
    friend network_worker;

public:
    inetwork_imp(void);
    ~inetwork_imp(void);

public:
    //----------------------------------------------------------------------
    // 接口实现
    virtual bool init_network(inet_tcp_handler* handler, 
        size_t thread_num = 1,
        size_t worker_num = 7) ;

    virtual bool try_write(net_conn* pconn, const char* buff, size_t len);          ///< 尝试发送数据
    virtual bool try_read(net_conn* pConn) ;                                        ///< 读

    virtual net_conn* try_listen(USHORT local_port) ;                                  ///< 监听端口
    virtual net_conn* try_connect(const char* addr, USHORT uport, void* bind_key) ;    ///< 投递连接到服务器

protected:
    //----------------------------------------------------------------------
    // 内部异步投递函数
    virtual net_conn* post_accept(net_conn* pListenConn) ;	                        ///< 投递接受请求,可多次投递，监听套接字必须用 create_listen_conn 创建
    virtual bool post_connection(net_conn* pConn) ;		                            ///< 投递连接请求
    virtual bool post_read(net_conn* pConn) ;	                                    ///< 投递读请求，在接受连接后底层会默认投递一个请求
    //virtual bool post_write(net_conn* pConn, char* buff, size_t len) ;	        ///< 投递写请求
    virtual bool post_placement_write(net_conn* pConn, char* buff, size_t len) ;	///< 投递写请求

protected:
    //----------------------------------------------------------------------
    // 处理完成事件
    void on_read(net_conn*, const char* buff, DWORD dwtransfer);
    void on_write(net_conn*, DWORD dwByteTransfered);
    void on_accept(net_conn* listen_conn, net_conn* accept_conn);
    void on_connect(net_conn*, bool bsuccess);

protected:
    //----------------------------------------------------------------------
    // net_conn 资源管理 
    virtual net_conn* create_listen_conn(USHORT uLocalPort) ;	            ///< 创建返回监听套接字，返回一个net_conn结构，用于接受连接
    net_overLapped* get_net_overlapped();                                   ///< 申请重叠结构

    //----------------------------------------------------------------------
    // 定时器相关

    //----------------------------------------------------------------------
    // 
    bool init_threads(size_t thread_num) ;                 ///< 启动网络层-》创建线程
    bool init_workers(size_t worker_num);

    network_worker* get_workder(void*);

protected:
    void worker_thread_(void* param);                 ///< 工作线程
    void check_and_disconnect(net_conn* pConn);                             ///< 判断在套接字上还有没有未处理的投递请求，如果没有了则断开连接

private:
    // handle
    HANDLE                  hiocp_;                                         ///< 完成端口句柄
    inet_tcp_handler*       net_event_handler_;                             ///< 移交上层处理的接口
    LPFN_ACCEPTEX           m_lpfnAcceptEx;                                 ///< AcceptEx函数指针
    bool                    m_bstop;

    thread_vec_type         work_threads_;
    size_t                  thread_num_;

    network_worker*         worker_arr_;
    size_t                  worker_num_;

    CObjectPool<net_overLapped, 1000, 1000>     net_overlapped_pool_;       // overlapped 
    CObjectPool<net_conn, 1000, 1000>           net_conn_pool_;             // net_conn
    
};