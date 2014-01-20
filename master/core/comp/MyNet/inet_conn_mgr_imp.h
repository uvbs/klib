#pragma once

#include "inet_conn_mgr.h"
#include <list>
#include <map>

#include <core/lock_stl.h>

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

    virtual size_t get_conn_count() ;

protected:
    klib::stl::lock_bucket_map<net_conn*, 
        net_conn*, 
        NETCONNECTION_ARRAY_LENGTH>  
        conn_list_x_;

};
