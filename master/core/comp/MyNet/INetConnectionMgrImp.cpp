#include "StdAfx.h"

#include "INetConnection.h"
#include "INetConnectionMgrImp.h"


INetConnectionMgrImp::INetConnectionMgrImp(void)
{
}

INetConnectionMgrImp::~INetConnectionMgrImp(void)
{
}

bool INetConnectionMgrImp::AddConnection(INetConnection* pConn) 
{
    int ipos = ConnHashFun(pConn, NETCONNECTION_ARRAY_LENGTH);
    {
        auto_lock helper(m_cs[ipos]);
        ConnectionListType::const_iterator itr;
        itr = m_ConnList[ipos].find(pConn);
        if (itr != m_ConnList[ipos].end()) {
            return false;
        }

        m_ConnList[ipos][pConn] = pConn;
    }
    return true;
}

bool INetConnectionMgrImp::RemoveConnection(INetConnection* pConn) 
{
    int ipos = ConnHashFun(pConn, NETCONNECTION_ARRAY_LENGTH);
    {
        auto_lock helper(m_cs[ipos]);
        ConnectionListType::const_iterator itr;
        itr = m_ConnList[ipos].find(pConn);
        if (itr != m_ConnList[ipos].end()) {
            m_ConnList[ipos].erase(itr);
            return true;
        }
    }

    return false;
}

bool INetConnectionMgrImp::IsExitsConnection(INetConnection* pConn) 
{
    int ipos = ConnHashFun(pConn, NETCONNECTION_ARRAY_LENGTH);
    {
        auto_lock helper(m_cs[ipos]);
        ConnectionListType::const_iterator itr;
        itr = m_ConnList[ipos].find(pConn);
        if (itr != m_ConnList[ipos].end()) {
            return true;
        }
    }

    return false;
}

int INetConnectionMgrImp::GetConnectionCount()
{
    int iCount = 0;

    for (int i=0; i<NETCONNECTION_ARRAY_LENGTH; ++i) 
    {
        iCount += m_ConnList[i].size();
    }
    return iCount;
}

bool INetConnectionMgrImp::ForeachConnection(NetConnMgrCallBackFun* callback, void* param) 
{
    ConnectionListType::const_iterator itr;

    for (int i=0; i<NETCONNECTION_ARRAY_LENGTH; ++i) 
    {
        auto_lock helper(m_cs[i]);

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