#include "StdAfx.h"
#include "INetPacketMgrImp.h"
#include "NetPacket.h"
#include "INetConnection.h"

INetPacketMgrImp::INetPacketMgrImp(void)
{
    m_iReadPacketPos = 0;

    InitPacketMgr();
}

INetPacketMgrImp::~INetPacketMgrImp(void)
{
}

bool INetPacketMgrImp::AddPacket(NetPacket* pPacket) 
{
    int iPos =  HashFun(pPacket->pConn);
    auto_lock helper(m_CsCombinedList[iPos]);

    NetConnMapType::const_iterator itrMap;
    stConnPacketHash& stPair = m_CombinedConnList[iPos];
    itrMap = stPair.connMapExists.find(pPacket->pConn);
    if (itrMap == stPair.connMapExists.end()) 
    {
        //没有存在则添加
        stPacketStore* pStore = new stPacketStore;
        if (NULL == pStore) 
        {
            return false;
        }

        pStore->pConn = pPacket->pConn;
        pStore->m_packetList.push_back(pPacket);
        stPair.connQueue.push_back(pStore);
        stPair.connMapExists[pPacket->pConn] = pStore;
    }
    else 
    {
        //添加封包
        itrMap->second->m_packetList.push_back(pPacket);
    }

    return true;
}

NetPacket* INetPacketMgrImp::GetPacket(bool bRemoveFlag/* = true*/) 
{
    NetPacket* pPacket = NULL;
    int ipos = 0;

    stPacketStore* stStore = NULL;
    int  iRepeatCount = 0;

    do
    {
        ++ iRepeatCount;
        m_iReadPacketPos %= NET_MAX_NETPACKET_LIST_LENGTH;
        ipos = m_iReadPacketPos;
        ++ m_iReadPacketPos;
        if (m_CombinedConnList[ipos].connMapExists.empty()) {
            continue;
        }

        auto_lock helper(m_CsCombinedList[ipos]);
        stConnPacketHash& stpair = m_CombinedConnList[ipos];
        BOOL bRepeatFlag = FALSE;

        do 
        {
            bRepeatFlag = FALSE;
            if (!stpair.connQueue.empty()) 
            {
                //队列中还有没有访问过的
                stStore = stpair.connQueue.front();     //取得最前一个未处理连接
                stpair.connQueue.pop_front();     //将该连接弹出队列
                stpair.connMapVisited[stStore->pConn] = stStore;    //保存到已经访问了的列表
                if (!stStore->m_packetList.empty()) 
                {
                    //该连接下有封包未处理
                    pPacket = stStore->m_packetList.front();
                    stStore->m_packetList.pop_front();
                }
                else 
                {
                    //该连接下没有封包被处理，需要再次取该队列的数据
                    bRepeatFlag = TRUE;
                }
            }
            else {
                //全访问过了，将访问过的全部添加到队列中去
                NetConnMapType::const_iterator itrRebuild;
                itrRebuild = stpair.connMapVisited.begin();
                for (; itrRebuild != stpair.connMapVisited.end(); ++itrRebuild)
                {
                    stpair.connQueue.push_back(itrRebuild->second);
                    if (!itrRebuild->second->m_packetList.empty()) 
                    {
                        bRepeatFlag = TRUE;
                    }
                }      
                stpair.connMapVisited.clear();
            }
        } while (bRepeatFlag);

    } while (NULL == pPacket && iRepeatCount < NET_MAX_NETPACKET_LIST_LENGTH + 1);

    return pPacket;
}

bool INetPacketMgrImp::FreeConnPacket(INetConnection* pConn)
{
    _ASSERT(pConn);
    int ipos =  HashFun(pConn);
    auto_lock helper(m_CsCombinedList[ipos]);

    NetConnMapType::const_iterator itrMap;
    stConnPacketHash& stpair = m_CombinedConnList[ipos];
    itrMap = stpair.connMapExists.find(pConn);
    if (itrMap == stpair.connMapExists.end()) 
    {
        //不存在的就不需要释放
        return false;
    }
    else 
    {
        //存在，需要从队列，map中删除，最后从exist中删除
        NetConnMapType::const_iterator itrVistMap;
        NetConnListType::const_iterator itrConnList;
        NetPacketListType::const_iterator itrPackList;
        stPacketStore* stStore = NULL;

        stpair.connMapExists.erase(itrMap);
        itrVistMap = stpair.connMapVisited.find(pConn);
        if (itrVistMap != stpair.connMapVisited.end()) 
        {
            //删除已访问列表中的数据
            stStore = itrVistMap->second;
            stpair.connMapVisited.erase(itrVistMap);
        }
        else 
        {
            //删除在队列中的数据(这个速度稍慢些，考虑如何改进)
            itrConnList = stpair.connQueue.begin();
            for (; itrConnList != stpair.connQueue.end(); ++itrConnList)
            {
                if ((*itrConnList)->pConn == pConn) 
                {
                    stStore = *itrConnList;
                    stpair.connQueue.erase(itrConnList);
                    break;
                }
            }
        }

        if (stStore) 
        {
            //释放连接下的封包
            itrPackList = stStore->m_packetList.begin();
            for (; itrPackList != stStore->m_packetList.end(); ++itrPackList) 
            {
                FreeNetPacket(*itrPackList);
            }
            stStore->m_packetList.clear();
            delete stStore;
        }
    }

    return false;
}

NetPacket* INetPacketMgrImp::AllocNetPacket() 
{
    NetPacket* pPacket = NULL;

    auto_lock helper(m_CsAllocedList);

    // 如果空闲列表中有则从空闲列表中获取，并初始化
    // 否则就从内存中申请
    if (!m_FreePacketList.empty())
    {
        pPacket = m_FreePacketList.front();

        BOOL bFixed = pPacket->m_bFixed;
        new(pPacket) NetPacket;
        pPacket->m_bFixed = bFixed;

        m_FreePacketList.pop_front();
    }
    else 
    {
        pPacket = new NetPacket;
    }

    return pPacket;
}

bool INetPacketMgrImp::FreeNetPacket(NetPacket* pPacket) 
{
    _ASSERT(pPacket);
    auto_lock helper(m_CsAllocedList);

    // 空闲列表中的对象比较多的时候要将非固定申请的内存给释放掉
    if (m_FreePacketList.size() > NET_MAX_PACKET_COUNT)
    {
        if (!pPacket->m_bFixed)
        {
            delete pPacket;
            return true;
        }
        else 
        {
            m_FreePacketList.push_back(pPacket);
        }
    }
    else 
    {
        m_FreePacketList.push_back(pPacket);
    }

    return true;
}

bool INetPacketMgrImp::InitPacketMgr(UINT uInitPacketNum/* = 300*/) 
{
    auto_lock helper(m_CsAllocedList);

    NetPacket* buff = new NetPacket[uInitPacketNum];
    if (NULL == buff) 
    {
        _ASSERT(FALSE && "out of memory!!!");
        return false;
    }

    // 将申请的内存保存到链表中，先不初始化，在使用的时候初始化(placement new)
    NetPacket* pPacket = NULL;
    for (UINT i=0; i<uInitPacketNum; ++ i) 
    {
        pPacket = (NetPacket*) & buff[i];
        pPacket->m_bFixed = TRUE;
        m_FreePacketList.push_back(pPacket);
    }

    return true;
}

int INetPacketMgrImp::HashFun(void* param)
{
    return ((int)param % NET_MAX_NETPACKET_LIST_LENGTH);
}