#include "CheckNat.h"
#include "stdio.h"
//#include "Log.h"

#pragma comment( lib, "Ws2_32.lib" )
#pragma warning(disable: 4996)

#define PORT_MAIN            6666
#define PORT_ASSIST            6667
#define ADDR_MAIN            "222.xx.xxx.xx"
#define ADDR_ASSIST            "222.xx.xxx.xx"


namespace klib {
namespace net {




CRITICAL_SECTION CNatCheck::m_section;

CNatCheck::CNatCheck()
{
    WSADATA wsaData;
    if ( 0 != WSAStartup(MAKEWORD(2,2), &wsaData) )
    {
        printf( "WSAStartup failed with (%d)\n", WSAGetLastError() );
    }

    InitializeCriticalSection( &m_section );
}

CNatCheck::~CNatCheck()
{
    DeleteCriticalSection( &m_section );

    WSACleanup();    
}

/********************************************************************
服务端部分
*********************************************************************/

DWORD WINAPI
    ServerThread(
    IN    LPVOID    lpParam
    )
{
    SOCKET s = ( SOCKET ) lpParam;

    sockaddr_in peer;
    int npeerlen = sizeof( peer );
    NatUDPDATA data;

    for (;;)
    {
        ZeroMemory( &data, NatUDPDATASize() );

        int nRet = recvfrom( s, (char*) &data, NatUDPDATASize(), 0, (sockaddr *)&peer, &npeerlen );
        if ( NatUDPDATASize() != nRet )
        {
            printf( "recvfrom packet failed with (%d)\n", WSAGetLastError() );
        }

        EnterCriticalSection( &CNatCheck::m_section );

        switch( data.nReqCode )
        {
        case REQ_GETIP:
            {
                printf( "REQ:REQ_GETIP\n" );
                NatAddress addr;
                addr.uPort = peer.sin_port;
                addr.uIP = peer.sin_addr.S_un.S_addr;

                NatUDPDATA addrData;
                CopyMemory( &addrData.peerClient, &addr, NatAddressSize() );

                nRet = sendto( s, (char*) &addrData, NatUDPDATASize(), 0, (sockaddr*) &peer, sizeof( peer ) );
                if ( SOCKET_ERROR == nRet )
                {
                    printf( "sendto failed with %d\n", WSAGetLastError() );
                }

                if ( NatUDPDATASize() != nRet )
                {
                    printf( "sendto NatAddressSize failed with %d\n", WSAGetLastError() );
                }
            }
            break;
        case REQ_CONNECT_SERVER:
            {
                printf( "Request to the assist server.\n" );

                /*
                *    向协助端口和ip发请求
                */

                sockaddr_in assistAddr;
                assistAddr.sin_family = AF_INET;
                assistAddr.sin_port = htons( PORT_ASSIST );
                assistAddr.sin_addr.S_un.S_addr = inet_addr( ADDR_ASSIST );

                NatUDPDATA dt;
                dt.nReqCode = REQ_CHECK;
                memcpy( &dt.peerClient, &data.peerClient, NatAddressSize() );

                nRet = sendto( s, (char*) &dt, NatUDPDATASize(), 0, (sockaddr*) &assistAddr, sizeof( assistAddr ) );
                if ( NatUDPDATASize() != nRet )
                {
                    printf( "sendto request assist failed with (%d)\n", WSAGetLastError() );
                }
            }
            break;
        case REQ_CHECK:
            {
                printf( "recv check request.\n" );

                sockaddr_in sendaddr;
                sendaddr.sin_family = AF_INET;
                sendaddr.sin_port = data.peerClient.uPort;
                sendaddr.sin_addr.S_un.S_addr = data.peerClient.uIP;

                NatUDPDATA datasend;
                datasend.peerClient.uPort = data.peerClient.uPort;
                datasend.peerClient.uIP = data.peerClient.uIP;

                nRet = sendto( s, (char*) &datasend, NatUDPDATASize(), 0, (sockaddr*) &sendaddr, sizeof( sendaddr ) );
                if ( NatUDPDATASize() != nRet )
                {
                    printf( "Sendto check client failed with %d\n", WSAGetLastError() );
                }
            }
            break;
        case REQ_OK:
            {
                char buffer[256];
                sprintf( buffer, "result => name:(%s), addr:(%d):(%d), type:(%d)\n",
                    data.szServerIP,
                    peer.sin_addr.S_un.S_addr,
                    peer.sin_port,
                    data.nType );

                printf( "%s", buffer );
                //LOG( "%s", buffer );
            }
            break;
        default:
            {
                printf( "Request command wrong!\n" );
            }
            break;
        }

        LeaveCriticalSection( &CNatCheck::m_section );
    }

    closesocket( s );

    return 0;
}

BOOL  CNatCheck::CreateSocket(
    IN    unsigned short nPort,
    IN    char* pIP,
    OUT    SOCKET& s
    )
{
    /*
    *    参数有效性判断！
    */

    if ( ( 0 >= nPort ) ||
        ( NULL == pIP ) ||
        ( 0 >= strlen( pIP ) ) )
    {
        printf( "Param invalid!\n" );

        return FALSE;
    }

    s = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( INVALID_SOCKET == s )
    {
        return FALSE;
    }

    sockaddr_in listenaddr;
    listenaddr.sin_family = AF_INET;
    listenaddr.sin_port = htons( nPort );
    listenaddr.sin_addr.S_un.S_addr = inet_addr( pIP );

    int nRet = bind( s, (sockaddr *) &listenaddr, sizeof(listenaddr) );
    if ( SOCKET_ERROR == nRet )
    {
        printf( "bind failed with (%d)\n", WSAGetLastError() );
        return FALSE;
    }

    return TRUE;
}

void  CNatCheck::ServerRun()
{
    /*
    *    创建两个监听socket
    */
    SOCKET sMain;
    BOOL bStatus = CreateSocket( PORT_MAIN, ADDR_MAIN, sMain );
    if ( !bStatus )
    {
        printf( "Create main socket failed with %d\n", WSAGetLastError() );
    }

    SOCKET sAssist;
    bStatus = CreateSocket( PORT_ASSIST, ADDR_ASSIST, sAssist );
    if ( !bStatus )
    {
        printf( "Create assist socket failed with %d\n", WSAGetLastError() );
    }

    DWORD dwID;    
    HANDLE h;

    /*
    *    分别启动线程来进行监听
    */

    h = CreateThread( NULL, 0, ServerThread, (PVOID) sMain, 0, &dwID );
    CloseHandle( h );
    h = CreateThread( NULL, 0, ServerThread, (PVOID) sAssist, 0, &dwID );
    CloseHandle( h );
}

/********************************************************************
客户端部分
*********************************************************************/


/*
*    向对应服务器请求指令
*/

BOOL  CNatCheck::Request( 
    IN    SOCKET        s,
    IN    const char*    pServerIp,
    IN    int            nPort, 
    IN    int            nReqCode,
    OUT NatAddress&    peer,
    IN const char* pTargetServerIp
    )
{
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(nPort);
    serverAddr.sin_addr.S_un.S_addr = inet_addr( pServerIp );

    /*
    *    创建请求包
    */
    NatUDPDATA data;
    data.nReqCode = nReqCode;
    strcpy( data.szServerIP, pTargetServerIp );
    memcpy( &data.peerClient, &peer, NatAddressSize() );

    BOOL bSuccess = FALSE;

    for ( int i = 0; i < 10; i ++ )
    {
        int nRet = sendto( s, (char*) &data, NatUDPDATASize(), 0, (sockaddr*) &serverAddr, sizeof( serverAddr ) );
        if ( NatUDPDATASize() != nRet )
        {
            printf( "sendto server packet failed with (%d)\n", WSAGetLastError() );
        }

        // 发送完毕，稍等片刻。

        Sleep(100);

        sockaddr_in peeraddr;
        int peerlen = sizeof( peeraddr );

        NatUDPDATA datarecv;
        nRet = recvfrom( s, (char*) &datarecv, NatUDPDATASize(), 0, (sockaddr*) &peeraddr, &peerlen );
        if ( WSAETIMEDOUT == WSAGetLastError() )
        {
            if ( REQ_CONNECT_SERVER == nReqCode )
            {
                printf( "Wait for Assist server timeout(400ms) ^_^.\n" );
            }

            continue;
        }

        if ( NatUDPDATASize() != nRet )
        {
            printf( "recvfrom packet failed with %d\n", WSAGetLastError() );
            continue;
        }

        bSuccess = TRUE;

        peer.uIP = datarecv.peerClient.uIP;
        peer.uPort = datarecv.peerClient.uPort;

        break;
    }

    return bSuccess;
}

NatType  CNatCheck::NatCheck()
{
    NatType nNatType = TYPE_BLOCK;

    // 获取本地地址
    GetLocalAddress();

    // 创建连接socket
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if ( INVALID_SOCKET == s )
    {
        printf( "Create socket failed with %d\n", WSAGetLastError() );
        return nNatType;
    }

    // 设置接收超时
    int nTimeO = 400;
    int nRet = setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, (char*)&nTimeO, sizeof( nTimeO ) );
    if ( SOCKET_ERROR == nRet )
    {
        printf( "setsockopt SO_RCVTIMEO failed with %d\n", WSAGetLastError() );
    }

    // 请求自己的public ip
    NatAddress peer1;
    if ( !Request( s, ADDR_MAIN, PORT_MAIN, REQ_GETIP, peer1, "" ) )
    {
        printf( "Requst to server1 falied with %d\n", WSAGetLastError() );
        return nNatType;
    }

    // 如果public = local ， 那么是公网地址。

    if ( ( peer1.uIP == m_local.uIP ) &&
        ( peer1.uPort == m_local.uPort ) )
    {
        nNatType = TYPE_PUBLIC;
        printf( "Node: TYPE_PUBLIC\n" );
    }
    else
    {
        // 否则是nat

        // 让Main告诉Assist，让它来跟我联系，如果我收到联系了，那么我是Full cone。

        NatAddress peer2;
        memcpy( &peer2, &peer1, NatAddressSize() );
        if ( Request( s, ADDR_MAIN, PORT_MAIN, REQ_CONNECT_SERVER, peer2, ADDR_ASSIST ) )
        {
            printf( "Node: TYPE_FULL_CONE\n" );
            nNatType = TYPE_FULL_CONE;
        }
        else
        {
            // 否则测试是限制型还是对称型


            // 向Assist取我的public ip & port

            ZeroMemory( &peer2, 0 );
            if ( !Request( s, ADDR_ASSIST, PORT_ASSIST, REQ_GETIP, peer2, "" ) )
            {
                printf( "Requst to server2 falied with %d\n", WSAGetLastError() );
                return nNatType;
            }

            if ( ( peer1.uIP != peer2.uIP ) ||
                ( peer2.uPort != peer2.uPort ) )
            {
                // 如果和Main取来的都不一样，那么是对称型的Nat。

                printf( "Node: TYPE_SYMMETRIC\n" );
                nNatType = TYPE_SYMMETRIC;
            }
            else
            {
                // 否则是限制型的，那么都可以穿透。
                // 注：还要看逻辑，详细参考我的博客: http://hi.baidu.com/sadusaga 关于Nat穿透的描述。

                printf( "Node: TYPE_RESTRICTED_CONE\n" );
                nNatType = TYPE_RESTRICTED_CONE;
            }
        }
    }

    NatUDPDATA ud;
    strcpy( ud.szServerIP, szName );
    ud.nReqCode = REQ_OK;
    ud.nType = nNatType;

    /*
    *    向主服务器发送检测结果
    */

    sockaddr_in mailaddr;
    mailaddr.sin_family = AF_INET;
    mailaddr.sin_port = htons(PORT_MAIN);
    mailaddr.sin_addr.S_un.S_addr = inet_addr( ADDR_MAIN );
    nRet = sendto( s, (char*) &ud, NatUDPDATASize(), 0, (sockaddr*) &mailaddr, sizeof( mailaddr ) );
    if ( nRet != NatUDPDATASize() )
    {
        printf( "sendto main server result failed with %d\n", WSAGetLastError() );
    }

    closesocket( s );

    printf( "\nCheck sucess!\n" );

    return nNatType;
}

void  CNatCheck::GetLocalAddress()
{
    /*
    *    获取本地IP地址
    */

    ZeroMemory( szName, 128 );
    gethostname( szName, 128 );
    struct hostent *hp = gethostbyname( szName );
    char** pip = hp->h_addr_list;
    BYTE b1 = pip[0][0];
    BYTE b2 = pip[0][1];
    BYTE b3 = pip[0][2];
    BYTE b4 = pip[0][3];
    char chHostIP[16];
    sprintf( chHostIP, "%d.%d.%d.%d", b1, b2, b3, b4 );

    m_local.uIP = inet_addr( chHostIP );
}



}}

