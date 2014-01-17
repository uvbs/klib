#include "StdAfx.h"

#include "net_conn.h"
#include "inet_conn_mgr_imp.h"


inet_conn_mgr_imp::inet_conn_mgr_imp(void)
{
}

inet_conn_mgr_imp::~inet_conn_mgr_imp(void)
{
}

bool inet_conn_mgr_imp::add_conn(net_conn* pConn) 
{
    return conn_list_x_.add_item(pConn, pConn);
}

bool inet_conn_mgr_imp::del_conn(net_conn* pConn) 
{
    return conn_list_x_.remove_item(pConn);
}

bool inet_conn_mgr_imp::is_exist_conn(net_conn* pConn) 
{
    return conn_list_x_.find_item(pConn);
}

size_t inet_conn_mgr_imp::get_conn_count()
{
    return conn_list_x_.size();
}
