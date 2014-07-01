#include "StdAfx.h"

#include "net_conn.h"
#include "net_conn_mgr_imp.h"
#include "network_imp.h"


net_conn_mgr_i_imp::net_conn_mgr_i_imp()
{
    conn_timeout_ = 2*60 * 1000;

    tmr_mgr_.start();
    tmr_mgr_.add(1000, std::bind(&net_conn_mgr_i_imp::check_valid_conn, this));
}

net_conn_mgr_i_imp::~net_conn_mgr_i_imp(void)
{
}

bool net_conn_mgr_i_imp::add_conn(net_conn_weak_ptr pConn) 
{
    auto ptr = pConn.lock();
    ptr->tmout_id_ = conn_tmout_checker_.insert(pConn);
    return conn_list_x_.add_item(ptr.get(), pConn);
}

bool net_conn_mgr_i_imp::del_conn(net_conn_weak_ptr pConn) 
{
    klib::kthread::guard guard_(mutex_);

    KLIB_ASSERT(pConn.lock()->tmout_id_ > 0);
    auto ptr = pConn.lock();
    conn_tmout_checker_.remove(ptr->tmout_id_);
    return conn_list_x_.remove_item(ptr.get());
}

bool net_conn_mgr_i_imp::is_exist_conn(const net_conn_weak_ptr& pConn) 
{
    return conn_list_x_.find_item(pConn.lock().get());
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
        [&](net_conn_weak_ptr pconn) -> bool
    {
        if (!is_exist_conn(pconn)) 
        {
            return false;
        }

        net_conn_ptr ptr = pconn.lock();
        if (nullptr == ptr) {
            return false;
        }

        DWORD dwtick = GetTickCount();
        if (dwtick - ptr->get_last_active_tm() >= 60*1000) 
        {
            // 如果超时了直接关闭连接
            ptr->dis_connect();
            return false;
        }

        // 没有超过的话继续检测
        return true;
    });

    return true;
}