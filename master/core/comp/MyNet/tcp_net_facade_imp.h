#pragma once

#include "tcp_net_facade.h"
#include <vector>


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
    virtual bool set_net_conn_mgr(inet_conn_mgr* pMgr) ;                //连接管理器
    virtual bool init();                                         //初始化客户端

    //////////////////////////////////////////////////////////////////////////
    // 获取其它接口
    virtual inetpacket_mgr* get_net_packet_mgr() { return net_packet_mgr_;}         //返回网络封包管理接口
    virtual inetwork*       get_network() { return inetwork_; }					//返回网络层接口
    virtual inet_conn_mgr*  get_net_conn_mgr() { return net_conn_mgr_; }	        //返回连接管理接口

    //////////////////////////////////////////////////////////////////////////
    // 事件处理接口
    virtual bool add_event_handler(inet_tcp_handler* handler) ;
    virtual bool del_event_handler(inet_tcp_handler* handler) ;

protected:
    //----------------------------------------------------------------------
    // 网络消息处理器
    virtual bool on_connect(net_conn* pConn, bool bConnected = true) ;
    virtual bool on_disconnect(net_conn* pConn) ;
    virtual bool on_read(net_conn* pConn, const char* buff, size_t len);
    virtual bool on_write(net_conn* pConn, size_t len) ;
    virtual bool on_accept(net_conn* pListen, net_conn* pNewConn, bool bSuccess = true) ;

protected:
    bool                        init_success_;                      //表示是否初始化成功
    icombiner*                  icombiner_;                         //包完整性判断,基于应用层的协议
    dispatcher_handler*         dispatch_handler_;                  //派遣接口
    inetwork*                   inetwork_;                          //网络接口
    inetpacket_mgr*             net_packet_mgr_;                    //封包管理器
    inet_conn_mgr*              net_conn_mgr_;                      //网络连接管理器

    mutex                       mutex_;                             //同步结构

    typedef std::vector<inet_tcp_handler*> INetEventHandlerListType;
    INetEventHandlerListType    net_event_list_;                    //事件处理器列表
};