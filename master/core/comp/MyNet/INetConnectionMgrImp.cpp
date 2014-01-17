#include "StdAfx.h"

#include "net_conn.h"
#include "INetConnectionMgrImp.h"


INetConnectionMgrImp::INetConnectionMgrImp(void)
{
}

INetConnectionMgrImp::~INetConnectionMgrImp(void)
{
}

bool INetConnectionMgrImp::add_conn(net_conn* pConn) 
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

bool INetConnectionMgrImp::rmv_conn(net_conn* pConn) 
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

bool INetConnectionMgrImp::is_exist_conn(net_conn* pConn) 
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

int INetConnectionMgrImp::get_conn_count()
{
    int iCount = 0;

    for (int i=0; i<NETCONNECTION_ARRAY_LENGTH; ++i) 
    {
        iCount += m_ConnList[i].size();
    }
    return iCount;
}

bool INetConnectionMgrImp::for_each_conn(conn_callback* callback, void* param) 
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

int INetConnectionMgrImp::ConnHashFun(void* param, int len)
{
    int ipos = (int)param % len;
    _ASSERT(ipos >=0 && ipos < NETCONNECTION_ARRAY_LENGTH);
    return ipos;
}