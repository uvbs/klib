#include "StdAfx.h"
#include "inetpacket_mgr_imp.h"
#include "net_packet.h"
#include "net_conn.h"

inetpacket_mgr_imp::inetpacket_mgr_imp(void)
{
    read_packet_pos_ = 0;

}

inetpacket_mgr_imp::~inetpacket_mgr_imp(void)
{
}

bool inetpacket_mgr_imp::add_packet(net_packet* pPacket) 
{
    int iPos =  _hash_func(pPacket->pConn);
    auto_lock helper(combin_list_mutex_[iPos]);

    NetConnMapType::const_iterator itrMap;
    stConnPacketHash& stPair = combin_list_[iPos];
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
        pStore->net_packet_list_.push_back(pPacket);
        stPair.connQueue.push_back(pStore);
        stPair.connMapExists[pPacket->pConn] = pStore;
    }
    else 
    {
        //添加封包
        itrMap->second->net_packet_list_.push_back(pPacket);
    }

    return true;
}

net_packet* inetpacket_mgr_imp::get_packet(bool bRemoveFlag/* = true*/) 
{
    net_packet* pPacket = NULL;
    int ipos = 0;

    stPacketStore* stStore = NULL;
    int  iRepeatCount = 0;

    do
    {
        ++ iRepeatCount;
        read_packet_pos_ %= NET_MAX_NETPACKET_LIST_LENGTH;
        ipos = read_packet_pos_;
        ++ read_packet_pos_;
        if (combin_list_[ipos].connMapExists.empty()) {
            continue;
        }

        auto_lock helper(combin_list_mutex_[ipos]);
        stConnPacketHash& stpair = combin_list_[ipos];
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
                if (!stStore->net_packet_list_.empty()) 
                {
                    //该连接下有封包未处理
                    pPacket = stStore->net_packet_list_.front();
                    stStore->net_packet_list_.pop_front();
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
                    if (!itrRebuild->second->net_packet_list_.empty()) 
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

bool inetpacket_mgr_imp::free_conn_packets(net_conn* pConn)
{
    _ASSERT(pConn);
    int ipos =  _hash_func(pConn);
    auto_lock helper(combin_list_mutex_[ipos]);

    NetConnMapType::const_iterator itrMap;
    stConnPacketHash& stpair = combin_list_[ipos];
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
            itrPackList = stStore->net_packet_list_.begin();
            for (; itrPackList != stStore->net_packet_list_.end(); ++itrPackList) 
            {
                free_net_packet(*itrPackList);
            }
            stStore->net_packet_list_.clear();
            delete stStore;
        }
    }

    return false;
}

net_packet* inetpacket_mgr_imp::alloc_net_packet() 
{
    return packet_pool_.Alloc();
}

bool inetpacket_mgr_imp::free_net_packet(net_packet* pPacket) 
{
    _ASSERT(pPacket);

    packet_pool_.Free(pPacket);
    return true;
}

int inetpacket_mgr_imp::_hash_func(void* param)
{
    return ((int)param % NET_MAX_NETPACKET_LIST_LENGTH);
}