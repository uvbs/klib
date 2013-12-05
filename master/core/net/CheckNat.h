// Author : Sadusaga
// Data: 2008.09.10
// 目的: NAT Check

//.h

#ifndef _NAT_CHECK_H_
#define _NAT_CHECK_H_


#include "windows.h"

typedef enum
{
    TYPE_BLOCK,
    TYPE_PUBLIC,
    TYPE_FIREWALL,
    TYPE_FULL_CONE,
    TYPE_SYMMETRIC,
    TYPE_RESTRICTED_CONE
} NatType;

enum
{
    REQ_GETIP,
    REQ_CONNECT_SERVER,
    REQ_CHECK,
    REQ_OK
};


typedef struct _NatAddress
{
    unsigned long uIP;
    unsigned short uPort;

} NatAddress, *PNatAddress;

#define NatAddressSize()        sizeof( NatAddress )


typedef struct _NatUDPDATA
{
    int nReqCode;
    int nType;

    char szServerIP[128];
    NatAddress peerClient;        

} NatUDPDATA, *NatPUDPDATA;
#define NatUDPDATASize()        sizeof( NatUDPDATA )

class CNatCheck
{
public:
    CNatCheck();
    ~CNatCheck();

public:
    // Server

    void ServerRun(
        IN void
        );


public:
    // Client

    NatType NatCheck(IN void );

private:

    // 服务端创建监听socket
    BOOL CreateSocket(
        IN    unsigned short nPort,
        IN    char* pIP,
        OUT    SOCKET& s
        );

    // 客户端请求
    BOOL Request( 
        IN    SOCKET        s, 
        IN    const char*    pServerIp, 
        IN    int            nPort, 
        IN    int            nReqCode, 
        OUT NatAddress& peer, 
        IN    const char*    pTargetServerIp
        );

    void GetLocalAddress(
        IN void
        );

public:

    static CRITICAL_SECTION m_section;

private:

    NatAddress m_local;
    char szName[128];

};

#endif