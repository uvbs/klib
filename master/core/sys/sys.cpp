#include "sys.h"
#include "..\core\scope_guard.h"

namespace klib{
namespace sys {

using namespace klib::core;


/*

//     TCHAR szApp[MAX_PATH * 2] = {0};
//     GetModuleFileName(0, szApp, _countof(szApp)); 
//     std::string szCommandLine = szApp;
//     szCommandLine = "\"";
//     szCommandLine += szApp;
//     szCommandLine += "\" ";
//     szCommandLine += "-autorun";
*/
bool system::SetAppRunBoot(const TCHAR* szAppName, const TCHAR* szCommandLine, size_t nLen, bool bSetBoot/* = true*/) 
{
    HKEY  regKey = NULL;


    RegOpenKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), &regKey);
    if (bSetBoot)
    {
        if(RegSetValueEx(regKey,
            szAppName,
            0,
            REG_SZ,
            (const unsigned char*) szCommandLine,
            nLen * sizeof(TCHAR)) != ERROR_SUCCESS)
        {
            RegCloseKey(regKey);
            return false;
        }
    }
    else 
    {
        if(RegDeleteValue(regKey, szAppName) != ERROR_SUCCESS)
        {
            RegCloseKey(regKey);
            return false;
        }
    }
    RegCloseKey(regKey);
    return true;
}


bool system::IsSetAppRunBoot(const TCHAR* szAppName)
{
    HKEY  regKey = NULL;
    ON_SCOPE_EXIT
    (
        if (regKey) 
        {
            RegCloseKey(regKey);
            regKey = NULL;
        }
    );

    LONG lRet = RegOpenKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), &regKey);
    if (ERROR_SUCCESS == lRet)
    {
        lRet = RegQueryValueEx(regKey, szAppName, 0, NULL, NULL, NULL);
        if (ERROR_SUCCESS == lRet) 
        {
            return true;
        }                       
        return false;
    }
    else 
    {
        return false;
    }
}

BOOL system::DebugPrivilegeEnable(BOOL Enable)
{
    HANDLE m_Token;
    if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &m_Token))
    {
        return false;
    }
    TOKEN_PRIVILEGES m_TokenP;
    m_TokenP.PrivilegeCount=1;
    if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &m_TokenP.Privileges[0].Luid))
    {
        return false;
    }
    m_TokenP.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;
    if(!AdjustTokenPrivileges(m_Token, false, &m_TokenP, sizeof(m_TokenP), 0, 0))
    {
        return false;
    } 
    CloseHandle(m_Token);
    return true;
}

static unsigned long g_OldProtect;
BOOL system::AddProtect(void *addr,int memSize)
{
    return VirtualProtectEx(::GetCurrentProcess(),addr,memSize,g_OldProtect,NULL);
}

BOOL system::RemoveProtect(void *addr,int memSize)
{
    return VirtualProtectEx(::GetCurrentProcess(),addr,memSize,PAGE_READWRITE,&g_OldProtect);
}

BOOL system::AddProtect(HANDLE processHandle,void *addr,unsigned long &proctect)
{
    return VirtualProtectEx(processHandle,addr,10,proctect,NULL);
}

BOOL system::RemoveProtect(HANDLE processHandle,void *addr,unsigned long &oldProctect)
{
    return VirtualProtectEx(processHandle,addr,10,PAGE_READWRITE,&oldProctect);
}

#define MAX_REMOTE_MEM (100)
void* system::InjectProcess(const char *dllName, HANDLE proHandle)
{
    int	len	= strlen(dllName);
    void* pMem = NULL;
    pMem = ::VirtualAllocEx(proHandle, NULL, MAX_REMOTE_MEM, MEM_COMMIT, PAGE_READWRITE);
    if(!pMem) return NULL;

    unsigned long rl;
    ::WriteProcessMemory(proHandle, pMem, dllName, len+1, &rl);
    HANDLE hThread = ::CreateRemoteThread(proHandle, NULL, 0,
        (unsigned long (__stdcall *)(void *))GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), ("LoadLibraryA")),
        pMem,0,0);

    if(!hThread)
    {
        ::VirtualFreeEx(proHandle, pMem, len + 1, MEM_COMMIT);
        return NULL;
    }
    CloseHandle(hThread);
    return pMem;
}


}}