#pragma once

#include "inetpacket_mgr.h"
#include <list>
#include <map>
#include <hash_map>
#include <queue>

#define  NET_MAX_NETPACKET_LIST_LENGTH 9                        // 定义hash数组的长度
#define  NET_MAX_PACKET_COUNT	5000                            // 封包管理中允许的最大个数

//----------------------------------------------------------------------
// Summary:
//		网络逻辑封包管理实现
//----------------------------------------------------------------------
class inetpacket_mgr_imp : public inetpacket_mgr
{
public:
    inetpacket_mgr_imp(void);
    ~inetpacket_mgr_imp(void);

public:
    virtual bool add_packet(net_packet* pPacket) ;                   ///< 将封包添加到封包管理器中
    virtual net_packet* get_packet(bool bRemoveFlag = true) ;        ///< 从网络封包管理器中获取一个生成好了的封包

    virtual bool free_conn_packets(net_conn* pConn);                ///< 释放一个连接对象下的所有封包

    virtual net_packet* alloc_net_packet() ;                         ///< 申请一个封包对象
    virtual bool free_net_packet(net_packet* pPacket) ;              ///< 释放网络封包

protected:
    bool InitPacketMgr(UINT uInitPacketNum = 900) ; //nCount表示初始封包个数
    int  HashFun(void* param);

protected:
    typedef std::list<net_packet*> NetPacketListType;

    //----------------------------------------------------------------------
    // Summary:
    //       表示一个连接下保存的多少网络封包,针对一个连接的封包都这样保存
    //----------------------------------------------------------------------
    struct stPacketStore{
        net_conn* pConn;
        NetPacketListType net_packet_list_;
    };

    typedef std::list<stPacketStore*> NetConnListType;
    typedef stdext::hash_map<net_conn*, stPacketStore*> NetConnMapType;
    struct stConnPacketHash {
        NetConnListType connQueue;                  // 以链表的形式保存所有的连接指针
        NetConnMapType  connMapVisited;             // 以map形式保存已经访问了的连接
        NetConnMapType  connMapExists;              // 以map形式保存所有的连接
    };

    stConnPacketHash     combin_list_[NET_MAX_NETPACKET_LIST_LENGTH];               ///< 以hash数组形式保存链接，以便快速查找
    mutex                combin_list_mutex_[NET_MAX_NETPACKET_LIST_LENGTH];         ///< 与上面对应的同步对象
    
    int                  read_packet_pos_;               ///< 表示上次读取到数组中的下标
        
    NetPacketListType    free_packet_list_;              ///< 申请封包申请与释放相关
    mutex                free_packet_list_mutex_;        ///< 同步对象 

};