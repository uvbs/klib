#ifndef _klib_tcp_net_facade_imp_h
#define _klib_tcp_net_facade_imp_h

#include "tcp_net_facade.h"
#include "network_i.h"
#include <vector>

#include <pattern/object_pool.h>
using namespace klib::pattern;

class tcp_net_facade_imp : public tcp_net_facade
{
public:
    tcp_net_facade_imp(void);
    ~tcp_net_facade_imp(void);

public:
    //////////////////////////////////////////////////////////////////////////
    // 初始设置接口
    virtual bool set_icombiner(icombiner* pCombiner) ;                  //设置分包处理接口
    virtual bool set_dispatch_handler(dispatcher_handler* pHandler) ;   //设置分派接口,如果不设置这个，则封包进入封包管理器中
    virtual bool init();                                         //初始化客户端

    //////////////////////////////////////////////////////////////////////////
    // 获取其它接口
    virtual network_i*       get_network() { return inetwork_; }					//返回网络层接口
    virtual net_conn_mgr_i*  get_net_conn_mgr() { return inetwork_->get_net_conn_mgr(); }	        //返回连接管理接口

    //////////////////////////////////////////////////////////////////////////
    // 事件处理接口
    virtual bool add_event_handler(inet_tcp_handler* handler) ;
    virtual bool del_event_handler(inet_tcp_handler* handler) ;

protected:
    //----------------------------------------------------------------------
    // 网络消息处理器
    virtual bool on_connect(net_conn_ptr pConn, bool bConnected = true) ;
    virtual bool on_disconnect(net_conn_ptr pConn) ;
    virtual bool on_read(net_conn_ptr pConn, const char* buff, size_t len);
    virtual bool on_write(net_conn_ptr pConn, size_t len) ;
    virtual bool on_accept(net_conn_ptr pListen, net_conn_ptr pNewConn, bool bSuccess = true) ;

protected:
    bool                        init_success_;                      //表示是否初始化成功
    icombiner*                  icombiner_;                         //包完整性判断,基于应用层的协议
    dispatcher_handler*         dispatch_handler_;                  //派遣接口
    network_i*                   inetwork_;                         //网络接口

    mutex                       mutex_;                             //同步结构

    typedef std::vector<inet_tcp_handler*> INetEventHandlerListType;
    INetEventHandlerListType    net_event_list_;                    //事件处理器列表
};

#endif
