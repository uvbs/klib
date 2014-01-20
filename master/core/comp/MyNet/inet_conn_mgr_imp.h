#pragma once

#include "inet_conn_mgr.h"
#include <list>
#include <map>

#include <core/lock_stl.h>
#include <util/timeout_checker.h>
#include <core/timer_mgr.h>
using namespace klib::util;

#define  NETCONNECTION_ARRAY_LENGTH 9

//----------------------------------------------------------------------
// 网络连接管理, @todo 添加上超时机制
class inet_conn_mgr_imp : public inet_conn_mgr
{
public:
    inet_conn_mgr_imp(void);
    ~inet_conn_mgr_imp(void);

public:
    virtual bool add_conn(net_conn* pConn) ;
    virtual bool del_conn(net_conn* pConn) ;

    virtual bool is_exist_conn(net_conn* pConn) ;
    virtual bool set_conn_timeout(size_t tm_seconds); ///< 设置连接超时时间
    virtual size_t get_conn_count() ;

protected:
    bool check_valid_conn();

protected:
    // time out
    size_t                          conn_timeout_;
    timeout_checker<net_conn*>      conn_tmout_checker_;

    timer_mgr                       tmr_mgr_;

protected:
    klib::stl::lock_bucket_map<net_conn*, 
        net_conn*, 
        NETCONNECTION_ARRAY_LENGTH>  
        conn_list_x_;

};
