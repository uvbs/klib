#pragma once

#include "INetPacketMgr.h"
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
class INetPacketMgrImp : public INetPacketMgr
{
public:
    INetPacketMgrImp(void);
    ~INetPacketMgrImp(void);

public:
    //----------------------------------------------------------------------
    // Summary:
    //		将封包添加到封包管理器中
    // Parameters:
    //		pPacket		-	生成的网络封包
    // Returns:
    //		添加成功返回true,失败返回false
    //----------------------------------------------------------------------
    virtual bool AddPacket(NetPacket* pPacket) ;


    //----------------------------------------------------------------------
    // Summary:
    //		从网络封包管理器中获取一个生成好了的封包
    // Parameters:
    //		bRemoveFlag		-	是否删除的标记
    // Returns:
    //		获取成功返回封包的指针，失败返回NULL
    //----------------------------------------------------------------------
    virtual NetPacket* GetPacket(bool bRemoveFlag = true) ;


    //----------------------------------------------------------------------
    // Summary:
    //		释放一个连接对象下的所有封包
    // Parameters:
    //		pConn		-	连接指针对象
    // Returns:
    //		释放完成返回true, 失败返回false
    //----------------------------------------------------------------------
    virtual bool FreeConnPacket(INetConnection* pConn);  

    //----------------------------------------------------------------------
    // Summary:
    //		申请一个封包对象
    // Returns:
    //		成功返回指针，失败返回NULL
    //----------------------------------------------------------------------
    virtual NetPacket* AllocNetPacket() ;


    //----------------------------------------------------------------------
    // Summary:
    //		释放网络封包
    // Parameters:
    //		pPacket		-	网络封包指针
    // Returns:
    //		释放成功返回true,失败返回false
    // Remark:
    //        释放到一个链表中或者释放内存
    //----------------------------------------------------------------------
    virtual bool FreeNetPacket(NetPacket* pPacket) ;

protected:
    bool InitPacketMgr(UINT uInitPacketNum = 900) ; //nCount表示初始封包个数
    int  HashFun(void* param);

protected:
    typedef std::list<NetPacket*> NetPacketListType;

    //----------------------------------------------------------------------
    // Summary:
    //       表示一个连接下保存的多少网络封包,针对一个连接的封包都这样保存
    //----------------------------------------------------------------------
    struct stPacketStore{
        INetConnection* pConn;
        NetPacketListType m_packetList;
    };

    typedef std::list<stPacketStore*> NetConnListType;
    typedef stdext::hash_map<INetConnection*, stPacketStore*> NetConnMapType;
    struct stConnPacketHash {
        NetConnListType connQueue;              // 以链表的形式保存所有的连接指针
        NetConnMapType  connMapVisited;         // 以map形式保存已经访问了的连接
        NetConnMapType  connMapExists;           // 以map形式保存所有的连接
    };

    //----------------------------------------------------------------------
    // Summary:
    //      以hash数组形式保存链接，以便快速查找
    //----------------------------------------------------------------------
    stConnPacketHash          m_CombinedConnList[NET_MAX_NETPACKET_LIST_LENGTH];

    //----------------------------------------------------------------------
    // Summary:
    //      与上面对应的同步对象
    //----------------------------------------------------------------------
    auto_cs                   m_CsCombinedList[NET_MAX_NETPACKET_LIST_LENGTH];

    //----------------------------------------------------------------------
    // Summary:
    //      表示上次读取到数组中的下标
    //----------------------------------------------------------------------
    int m_iReadPacketPos;

    //----------------------------------------------------------------------
    // Summary:
    //      申请封包申请与释放相关
    //----------------------------------------------------------------------
    NetPacketListType         m_FreePacketList;
    auto_cs                   m_CsAllocedList;

};