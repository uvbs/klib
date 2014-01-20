#include "StdAfx.h"

#include "net_conn.h"
#include "inet_conn_mgr_imp.h"


inet_conn_mgr_imp::inet_conn_mgr_imp(void)
{
    conn_timeout_ = 60 * 1000;

    tmr_mgr_.start();
    tmr_mgr_.add(1000, std::bind(&inet_conn_mgr_imp::check_valid_conn, this));
}

inet_conn_mgr_imp::~inet_conn_mgr_imp(void)
{
}

bool inet_conn_mgr_imp::add_conn(net_conn* pConn) 
{
    pConn->tmout_id_ = conn_tmout_checker_.insert(pConn);
    return conn_list_x_.add_item(pConn, pConn);
}

bool inet_conn_mgr_imp::del_conn(net_conn* pConn) 
{
    KLIB_ASSERT(pConn->tmout_id_ > 0);
    conn_tmout_checker_.remove(pConn->tmout_id_);
    return conn_list_x_.remove_item(pConn);
}

bool inet_conn_mgr_imp::is_exist_conn(net_conn* pConn) 
{
    return conn_list_x_.find_item(pConn);
}

bool inet_conn_mgr_imp::set_conn_timeout(size_t tm_seconds)
{
    conn_timeout_ = tm_seconds;
    return true;
}

size_t inet_conn_mgr_imp::get_conn_count()
{
    return conn_list_x_.size();
}

bool inet_conn_mgr_imp::check_valid_conn()
{
    conn_tmout_checker_.check(conn_timeout_,
        [&](net_conn* pconn)
    {
        if (!is_exist_conn(pconn)) {
            return ;
        }

        // 如果超时了直接关闭连接
        pconn->dis_connect();
    });

    return true;
}