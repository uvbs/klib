#pragma once

// 定义最大封包长度
#define  MAX_NETPACKET_LENTTH  4*1024

// 保存网络数据包的
class net_conn;
class NetPacket
{
public:
    NetPacket(void);
    ~NetPacket(void);

public:
    BOOL    m_bFixed;                   // 表示是否是固定的，即不需要被delete掉的
    net_conn*  pConn;	            // 属于哪个连接
    size_t datalen;                     // 缓存区中保存数据的大小
    char buff[MAX_NETPACKET_LENTTH];    // 缓冲区
};