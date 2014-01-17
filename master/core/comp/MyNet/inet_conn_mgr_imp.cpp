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
    int ipos = ConnHashFun(pConn, NETCONNECTION_ARRAY_LENGTH);
    {
        guard helper(mutexs_[ipos]);
        ConnectionListType::const_iterator itr;
        itr = m_ConnList[ipos].find(pConn);
        if (itr != m_ConnList[ipos].end()) {
            return false;
        }

        m_ConnList[ipos][pConn] = pConn;
    }
    return true;
}

bool inet_conn_mgr_imp::del_conn(net_conn* pConn) 
{
    int ipos = ConnHashFun(pConn, NETCONNECTION_ARRAY_LENGTH);
    {
        guard helper(mutexs_[ipos]);
        ConnectionListType::const_iterator itr;
        itr = m_ConnList[ipos].find(pConn);
        if (itr != m_ConnList[ipos].end()) {
            m_ConnList[ipos].erase(itr);
            return true;
        }
    }

    return false;
}

bool inet_conn_mgr_imp::is_exist_conn(net_conn* pConn) 
{
    int ipos = ConnHashFun(pConn, NETCONNECTION_ARRAY_LENGTH);
    {
        guard helper(mutexs_[ipos]);
        ConnectionListType::const_iterator itr;
        itr = m_ConnList[ipos].find(pConn);
        if (itr != m_ConnList[ipos].end()) {
            return true;
        }
    }

    return false;
}

int inet_conn_mgr_imp::get_conn_count()
{
    int iCount = 0;

    for (int i=0; i<NETCONNECTION_ARRAY_LENGTH; ++i) 
    {
        iCount += m_ConnList[i].size();
    }
    return iCount;
}

bool inet_conn_mgr_imp::for_each_conn(conn_callback* callback, void* param) 
{
    ConnectionListType::const_iterator itr;

    for (int i=0; i<NETCONNECTION_ARRAY_LENGTH; ++i) 
    {
        guard helper(mutexs_[i]);

        itr = m_ConnList[i].begin();
        for (; itr != m_ConnList[i].end(); ++itr)
        {
            (*callback)(itr->first, param);
        }
    }

    return true;
}

int inet_conn_mgr_imp::ConnHashFun(void* param, int len)
{
    int ipos = (int)param % len;
    _ASSERT(ipos >=0 && ipos < NETCONNECTION_ARRAY_LENGTH);
    return ipos;
}