#pragma once

#include "INetwork.h"
#include "net_socket.h"

#include <list>
#include <vector>
#include <Mswsock.h>

#include <core/timer_mgr.h>
#include <kthread/thread.h>
#include <kthread/thread_local.h>

using namespace klib::kthread;
typedef std::vector<Thread> thread_vec_type;

//----------------------------------------------------------------------
// 单句柄数据
//  处理网络相关数据
class net_overLapped: public OVERLAPPED
{
public:
    ///<  以下是用户需要的信息
    bool bFixed;                        ///< 表示是否是固定的内存，不允许释放

    //外部使用
    int       operate_type_;            ///< 提交的操作类型
    void*     pend_data_;               ///< 附带其它数据的
    DWORD     transfer_bytes_;          ///< 传输了的数据
    WSABUF    wsaBuf_;                  ///< 缓冲区对象
    char      recv_buff_[2 * 1024];		///< 接收数据的buf
    
public:
    net_overLapped() : bFixed(false), pend_data_(0), transfer_bytes_(0)
    {
    }
};


/// 网络库实现
class inetwork_imp : public inetwork
{
public:
    inetwork_imp(void);
    ~inetwork_imp(void);

public:
    //----------------------------------------------------------------------
    // 接口实现
    virtual bool init_network(inet_tcp_handler* handler, size_t thread_num = 1) ;   
    virtual bool run_network() ;                 ///< 启动网络层-》创建线程

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
    virtual net_conn* create_conn() ;			                            ///< 创建一个连接套接字
    virtual bool release_conn(net_conn* pConn);		                        ///< 释放连接

    //----------------------------------------------------------------------
    // overlapped 资源管理
    void init_fixed_overlapped_list(size_t nCount);                         ///< 初始固定的Overlapped的个数，这部分内存不能被释放
    net_overLapped* get_net_overlapped();                                   ///< 申请重叠结构
    bool release_net_overlapped(net_overLapped* pMyoverlapped);	            ///< 释放重叠结构

    //----------------------------------------------------------------------
    // 定时器相关

protected:
    void worker_thread_(void* param);                 ///< 工作线程
    void check_and_disconnect(net_conn* pConn);                             ///< 判断在套接字上还有没有未处理的投递请求，如果没有了则断开连接

private:
    // handle
    HANDLE                  hiocp_;                                         ///< 完成端口句柄
    inet_tcp_handler*       net_event_handler_;                             ///< 移交上层处理的接口
    LPFN_ACCEPTEX           m_lpfnAcceptEx;                                 ///< AcceptEx函数指针
    thread_vec_type         work_threads_;
    size_t                  thread_num_;

    // overlapped 
    typedef std::list<net_overLapped*> OverLappedListType;                  ///< 保存net_overLapped的链表类型
    OverLappedListType      overlapped_list_;                               ///< 申明链表，用来保存net_overLapped
    mutex                   overlapped_list_mutex_;                         ///< 同步访问overlapped_list_

    // net_conn
    typedef std::list<net_conn*> INetConnListType;                          ///< 接口列表类型定义
    INetConnListType        free_net_conn_list_;                            ///< 保存网络连接接口链表
    mutex                   free_net_conn_mutex_;                           ///< 同步访问free_net_conn_list_

    klib::core::timer_mgr   tmr_mgr_;
};