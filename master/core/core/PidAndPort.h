#ifndef _klib_PidAndPort_h
#define _klib_PidAndPort_h


#include <Windows.h> 
#include <Psapi.h>
#include <Iprtrmib.h>  
#include <iostream>  
#include <vector>  

namespace klib{
namespace core{


#pragma  comment(lib,"Psapi.lib")  
#pragma  comment(lib,"Iphlpapi.Lib")  
#pragma  comment(lib,"WS2_32.lib")  

//------------------------------------------------------------------------
// Remarks:
//      进程ID或端口相互查找(进程端口查看命令：Netstat -anb)
//------------------------------------------------------------------------
class pid_and_port
{
public:
    pid_and_port(void);
    ~pid_and_port(void);

    enum TcpOrUdp  
    {  
        TcpType,
        UdpType
    };

    typedef struct  
    {  
        DWORD dwState;      //连接状态  
        DWORD dwLocalAddr;  //本地地址  
        DWORD dwLocalPort;  //本地端口  
        DWORD dwRemoteAddr; //远程地址  
        DWORD dwRemotePort; //远程端口  
        DWORD dwProcessId;  //进程标识  
    }MIB_TCPEXROW,*PMIB_TCPEXROW;  


    typedef struct  
    {  
        DWORD dwLocalAddr;  //本地地址  
        DWORD dwLocalPort;  //本地端口  
        DWORD dwProcessId;  //进程标识  

    }MIB_UDPEXROW,*PMIB_UDPEXROW;  


    typedef struct  
    {  
        DWORD dwState;      //连接状态  
        DWORD dwLocalAddr;  //本地地址  
        DWORD dwLocalPort;  //本地端口  
        DWORD dwRemoteAddr; //远程地址  
        DWORD dwRemotePort; //远程端口  
        DWORD dwProcessId;  //进程标识  
        DWORD Unknown;      //待定标识  

    }MIB_TCPEXROW_VISTA,*PMIB_TCPEXROW_VISTA;  


    typedef struct  
    {  
        DWORD dwNumEntries;  
        MIB_TCPEXROW table[ANY_SIZE];  

    }MIB_TCPEXTABLE,*PMIB_TCPEXTABLE;  


    typedef struct  
    {  
        DWORD dwNumEntries;  
        MIB_TCPEXROW_VISTA table[ANY_SIZE];  

    }MIB_TCPEXTABLE_VISTA,*PMIB_TCPEXTABLE_VISTA;  


    typedef struct  
    {  
        DWORD dwNumEntries;  
        MIB_UDPEXROW table[ANY_SIZE];  

    }MIB_UDPEXTABLE,*PMIB_UDPEXTABLE;  


    //=====================================================================================//  
    //Name: DWORD AllocateAndGetTcpExTableFromStack()                                      //  
    //                                                                                     //  
    //Descripion: 该函数仅仅只在 Windows XP，Windows Server 2003 下有效                           //  
    //                                                                                     //  
    //=====================================================================================//  
    typedef DWORD (WINAPI *PFNAllocateAndGetTcpExTableFromStack)(  
        PMIB_TCPEXTABLE *pTcpTabel,  
        bool bOrder,  
        HANDLE heap,  
        DWORD zero,  
        DWORD flags  
        );  

    //=====================================================================================//  
    //Name: DWORD AllocateAndGetUdpExTableFromStack()                                      //  
    //                                                                                     //  
    //Descripion: 该函数仅仅只在 XP，Windows Server 2003 下有效                                   //  
    //                                                                                     //  
    //=====================================================================================//  
    typedef DWORD (WINAPI *PFNAllocateAndGetUdpExTableFromStack)(  
        PMIB_UDPEXTABLE *pUdpTable,  
        bool bOrder,  
        HANDLE heap,  
        DWORD zero,  
        DWORD flags  
        );  

    //=====================================================================================//  
    //Name: DWORD InternalGetTcpTable2()                                                   //  
    //                                                                                     //  
    //Descripion: 该函数在 Windows Vista 以及 Windows 7 下面效                                  //  
    //                                                                                     //  
    //=====================================================================================//  
    typedef DWORD (WINAPI *PFNInternalGetTcpTable2)(  
        PMIB_TCPEXTABLE_VISTA *pTcpTable_Vista,  
        HANDLE heap,  
        DWORD flags  
        );  

    //=====================================================================================//  
    //Name: DWORD InternalGetUdpTableWithOwnerPid()                                        //  
    //                                                                                     //  
    //Descripion: 该函数在 Windows Vista 以及 Windows 7 下面效                                  //  
    //                                                                                     //  
    //=====================================================================================//  
    typedef DWORD (WINAPI *PFNInternalGetUdpTableWithOwnerPid)(  
        PMIB_UDPEXTABLE *pUdpTable,  
        HANDLE heap,  
        DWORD flags  
        );  


    //=====================================================================================//  
    //Name: DWORD GetProcessIdByPort()                                                     //  
    //                                                                                     //  
    //Descripion: 根据端口号求出打开该端口号的进程 ID(支持 XP，Server 2003，Vista，Win7)            //  
    //                                                                                     //  
    //=====================================================================================//  
    DWORD GetProcessIdByPort(TcpOrUdp type, DWORD dwPort)  
    {  
        HMODULE hModule = LoadLibraryW(L"iphlpapi.dll");  
        if (hModule == NULL)  
        {  
            return 0;  
        }  

        if(type == TcpType)  
        {  
            // 表明查询的是 TCP 信息  
            PFNAllocateAndGetTcpExTableFromStack pAllocateAndGetTcpExTableFromStack;  
            pAllocateAndGetTcpExTableFromStack =   
                (PFNAllocateAndGetTcpExTableFromStack)GetProcAddress(hModule, "AllocateAndGetTcpExTableFromStack");  
            if (pAllocateAndGetTcpExTableFromStack != NULL)  
            {  
                // 表明为 XP 或者 Server 2003 操作系统  
                PMIB_TCPEXTABLE pTcpExTable = NULL;  
                if (pAllocateAndGetTcpExTableFromStack(&pTcpExTable, TRUE, GetProcessHeap(), 0, AF_INET) != 0)  
                {  
                    if (pTcpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return 0;  
                }  

                for (UINT i = 0; i < pTcpExTable->dwNumEntries; i++)  
                {  
                    // 过滤掉数据，只查询我们需要的进程数据  
                    if(dwPort == ntohs(0x0000FFFF & pTcpExTable->table[i].dwLocalPort))  
                    {  
                        DWORD dwProcessId = pTcpExTable->table[i].dwProcessId;  
                        if (pTcpExTable)  
                        {  
                            HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                        }  

                        FreeLibrary(hModule);  
                        hModule = NULL;  

                        return dwProcessId;  
                    }  
                }  

                if (pTcpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return 0;  
            }  
            else  
            {  
                // 表明为 Vista 或者 7 操作系统  
                PMIB_TCPEXTABLE_VISTA pTcpExTable = NULL;  
                PFNInternalGetTcpTable2 pInternalGetTcpTable2 =   
                    (PFNInternalGetTcpTable2)GetProcAddress(hModule, "InternalGetTcpTable2");  
                if (pInternalGetTcpTable2 == NULL)  
                {  
                    if (pTcpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return 0;  
                }  

                if (pInternalGetTcpTable2(&pTcpExTable, GetProcessHeap(), 1))  
                {  
                    if (pTcpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return 0;  
                }  

                for (UINT i = 0;i < pTcpExTable->dwNumEntries; i++)  
                {  
                    // 过滤掉数据，只查询我们需要的进程数据  
                    if(dwPort == ntohs(0x0000FFFF & pTcpExTable->table[i].dwLocalPort))  
                    {  
                        DWORD dwProcessId = pTcpExTable->table[i].dwProcessId;  
                        if (pTcpExTable)  
                        {  
                            HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                        }  

                        FreeLibrary(hModule);  
                        hModule = NULL;  

                        return dwProcessId;  
                    }  
                }  

                if (pTcpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return 0;  
            }  
        }  
        else if(type == UdpType)  
        {  
            // 表明查询的是 UDP 信息  
            PMIB_UDPEXTABLE pUdpExTable = NULL;  
            PFNAllocateAndGetUdpExTableFromStack pAllocateAndGetUdpExTableFromStack;  
            pAllocateAndGetUdpExTableFromStack =   
                (PFNAllocateAndGetUdpExTableFromStack)GetProcAddress(hModule,"AllocateAndGetUdpExTableFromStack");  
            if (pAllocateAndGetUdpExTableFromStack != NULL)  
            {  
                // 表明为 XP 或者 Server 2003 操作系统  
                if (pAllocateAndGetUdpExTableFromStack(&pUdpExTable, TRUE, GetProcessHeap(), 0, AF_INET) != 0)  
                {  
                    if (pUdpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return 0;  
                }  

                for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)  
                {  
                    // 过滤掉数据，只查询我们需要的进程数据  
                    if (dwPort == ntohs(0x0000FFFF & pUdpExTable->table[i].dwLocalPort))  
                    {  
                        DWORD dwProcessId = pUdpExTable->table[i].dwProcessId;  
                        if (pUdpExTable)  
                        {  
                            HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                        }  

                        FreeLibrary(hModule);  
                        hModule = NULL;  

                        return dwProcessId;  
                    }  
                }  

                if (pUdpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return 0;  
            }  
            else  
            {  
                // 表明为 Vista 或者 7 操作系统  
                PFNInternalGetUdpTableWithOwnerPid pInternalGetUdpTableWithOwnerPid;  
                pInternalGetUdpTableWithOwnerPid =   
                    (PFNInternalGetUdpTableWithOwnerPid)GetProcAddress(hModule, "InternalGetUdpTableWithOwnerPid");  
                if (pInternalGetUdpTableWithOwnerPid != NULL)  
                {  
                    if (pInternalGetUdpTableWithOwnerPid(&pUdpExTable, GetProcessHeap(), 1))  
                    {  
                        if (pUdpExTable)  
                        {  
                            HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                        }  

                        FreeLibrary(hModule);  
                        hModule = NULL;  

                        return 0;  
                    }  

                    for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)  
                    {  
                        // 过滤掉数据，只查询我们需要的进程数据  
                        if (dwPort == ntohs(0x0000FFFF & pUdpExTable->table[i].dwLocalPort))  
                        {  
                            DWORD dwProcessId = pUdpExTable->table[i].dwProcessId;  
                            if (pUdpExTable)  
                            {  
                                HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                            }  

                            FreeLibrary(hModule);  
                            hModule = NULL;  

                            return dwProcessId;  
                        }  
                    }  
                }  

                if (pUdpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return 0;  
            }  
        }  
        else  
        {  
            FreeLibrary(hModule);  
            hModule = NULL;  

            return -1;  
        }  
    }  


    //=====================================================================================//  
    //Name: DWORD GetAllPortByProcessId()                                                  //  
    //                                                                                     //  
    //Descripion: 根据进程 ID 来求出该进程所打开的所有的端口号，并且在 dwAllPort 数组中返回所有端口号    //  
    //            其中 dwMaxLen 为数组的长度，函数的返回值为进程所打开的端口的数目                    //  
    //            (支持 XP，Server 2003，Vista，Win7)                                           //  
    //                                                                                     //  
    //=====================================================================================//  
    DWORD GetAllPortByProcessId(TcpOrUdp type, DWORD dwProcessId, DWORD * dwAllPort, DWORD dwMaxLen)  
    {
        DWORD dwPortCount = 0;  
        HMODULE hModule = LoadLibraryW(L"iphlpapi.dll");  
        if (hModule == NULL)  
        {  
            return dwPortCount;  
        }  

        if(type == TcpType)  
        {  
            // 表明查询的是 UDP 信息  
            PFNAllocateAndGetTcpExTableFromStack pAllocateAndGetTcpExTableFromStack;  
            pAllocateAndGetTcpExTableFromStack = (PFNAllocateAndGetTcpExTableFromStack)GetProcAddress(hModule, "AllocateAndGetTcpExTableFromStack");  
            if (pAllocateAndGetTcpExTableFromStack != NULL)  
            {  
                // 表明为 XP 或者 Server 2003 操作系统  
                PMIB_TCPEXTABLE pTcpExTable = NULL;  
                if (pAllocateAndGetTcpExTableFromStack(&pTcpExTable, TRUE, GetProcessHeap(), 0, AF_INET) != 0)  
                {  
                    if (pTcpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return dwPortCount;  
                }  

                for (UINT i = 0; i < pTcpExTable->dwNumEntries; i++)  
                {  
                    // 过滤掉数据，只获取我们要查询的进程的 Port 信息  
                    if(dwProcessId == pTcpExTable->table[i].dwProcessId)  
                    {  
                        if(dwPortCount < dwMaxLen)  
                        {  
                            dwAllPort[dwPortCount] = ntohs(0x0000FFFF & pTcpExTable->table[i].dwLocalPort);  
                            dwPortCount++;  
                        }  
                    }  
                }  

                if (pTcpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return dwPortCount;  
            }  
            else  
            {  
                // 表明为 Vista 或者 7 操作系统  
                PMIB_TCPEXTABLE_VISTA pTcpExTable = NULL;  
                PFNInternalGetTcpTable2 pInternalGetTcpTable2 = (PFNInternalGetTcpTable2)GetProcAddress(hModule, "InternalGetTcpTable2");  
                if (pInternalGetTcpTable2 == NULL)  
                {  
                    if (pTcpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return dwPortCount;  
                }  

                if (pInternalGetTcpTable2(&pTcpExTable, GetProcessHeap(), 1))  
                {  
                    if (pTcpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return dwPortCount;  
                }  

                for (UINT i = 0;i < pTcpExTable->dwNumEntries; i++)  
                {  
                    // 过滤掉数据，只获取我们要查询的进程的 TCP Port 信息  
                    if(dwProcessId == pTcpExTable->table[i].dwProcessId)  
                    {  
                        if(dwPortCount < dwMaxLen)  
                        {  
                            dwAllPort[dwPortCount] = ntohs(0x0000FFFF & pTcpExTable->table[i].dwLocalPort);  
                            dwPortCount++;  
                        }  
                    }  
                }  

                if (pTcpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return dwPortCount;  
            }  
        }  
        else if(type == UdpType)  
        {  
            // 表明查询的是 UDP 信息  
            PMIB_UDPEXTABLE pUdpExTable = NULL;  
            PFNAllocateAndGetUdpExTableFromStack pAllocateAndGetUdpExTableFromStack;  
            pAllocateAndGetUdpExTableFromStack = (PFNAllocateAndGetUdpExTableFromStack)GetProcAddress(hModule,"AllocateAndGetUdpExTableFromStack");  
            if (pAllocateAndGetUdpExTableFromStack != NULL)  
            {  
                // 表明为 XP 或者 Server 2003 操作系统  
                if (pAllocateAndGetUdpExTableFromStack(&pUdpExTable, TRUE, GetProcessHeap(), 0, AF_INET) != 0)  
                {  
                    if (pUdpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return dwPortCount;  
                }  

                for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)  
                {  
                    // 过滤掉数据，只获取我们要查询的进程的 UDP Port信息  
                    if(dwProcessId == pUdpExTable->table[i].dwProcessId)  
                    {  
                        if(dwPortCount < dwMaxLen)  
                        {  
                            dwAllPort[dwPortCount] = ntohs(0x0000FFFF & pUdpExTable->table[i].dwLocalPort);  
                            dwPortCount++;  
                        }  
                    }  
                }  

                if (pUdpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return dwPortCount;  
            }  
            else  
            {  
                // 表明为 Vista 或者 7 操作系统  
                PFNInternalGetUdpTableWithOwnerPid pInternalGetUdpTableWithOwnerPid;  
                pInternalGetUdpTableWithOwnerPid = (PFNInternalGetUdpTableWithOwnerPid)GetProcAddress(hModule, "InternalGetUdpTableWithOwnerPid");  
                if (pInternalGetUdpTableWithOwnerPid != NULL)  
                {  
                    if (pInternalGetUdpTableWithOwnerPid(&pUdpExTable, GetProcessHeap(), 1))  
                    {  
                        if (pUdpExTable)  
                        {  
                            HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                        }  

                        FreeLibrary(hModule);  
                        hModule = NULL;  

                        return dwPortCount;  
                    }  

                    for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)  
                    {  
                        // 过滤掉数据，只获取我们要查询的进程的 UDP Port信息  
                        if(dwProcessId == pUdpExTable->table[i].dwProcessId)  
                        {  
                            if(dwPortCount < dwMaxLen)  
                            {  
                                dwAllPort[dwPortCount] = ntohs(0x0000FFFF & pUdpExTable->table[i].dwLocalPort);  
                                dwPortCount++;  
                            }  
                        }  
                    }  
                }  

                if (pUdpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return dwPortCount;  
            }  
        }  
        else  
        {  
            FreeLibrary(hModule);  
            hModule = NULL;  

            return dwPortCount;  
        }  
    }  
};




}}



#endif
