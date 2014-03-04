#include "StdAfx.h"

#include "net_conn.h"
#include "net_conn_mgr_imp.h"


net_conn_mgr_i_imp::net_conn_mgr_i_imp(void)
{
    conn_timeout_ = 2*60 * 1000;

    tmr_mgr_.start();
    tmr_mgr_.add(1000, std::bind(&net_conn_mgr_i_imp::check_valid_conn, this));
}

net_conn_mgr_i_imp::~net_conn_mgr_i_imp(void)
{
}

bool net_conn_mgr_i_imp::add_conn(net_conn_ptr pConn) 
{
    pConn->tmout_id_ = conn_tmout_checker_.insert(pConn);
    return conn_list_x_.add_item(pConn.get(), pConn);
}

bool net_conn_mgr_i_imp::del_conn(net_conn_ptr pConn) 
{
    klib::kthread::guard guard_(mutex_);

    KLIB_ASSERT(pConn->tmout_id_ > 0);
    conn_tmout_checker_.remove(pConn->tmout_id_);
    return conn_list_x_.remove_item(pConn.get());
}

bool net_conn_mgr_i_imp::is_exist_conn(const net_conn_ptr& pConn) 
{
    return conn_list_x_.find_item(pConn.get());
}

bool net_conn_mgr_i_imp::set_conn_timeout(size_t tm_seconds)
{
    conn_timeout_ = tm_seconds;
    return true;
}

size_t net_conn_mgr_i_imp::get_conn_count()
{
    return conn_list_x_.size();
}

bool net_conn_mgr_i_imp::check_valid_conn()
{
    klib::kthread::guard guard_(mutex_);

    conn_tmout_checker_.check(conn_timeout_,
        [&](net_conn_ptr pconn) -> bool
    {
        if (!is_exist_conn(pconn)) 
        {
            return false;
        }

        DWORD dwtick = GetTickCount();
        if (dwtick - pconn->get_last_active_tm() >= 60*1000) 
        {
            // 如果超时了直接关闭连接
            pconn->dis_connect();
            return false;
        }

        // 没有超过的话继续检测
        return true;
    });

    return true;
}