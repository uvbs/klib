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

BOOL system::is_admin()
{
    BOOL bIsElevated = FALSE;
    HANDLE hToken    = NULL;
    UINT16 uWinVer   = LOWORD(GetVersion());
    uWinVer = MAKEWORD(HIBYTE(uWinVer),LOBYTE(uWinVer));
    if (uWinVer < 0x0600) return TRUE;
    if (OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hToken))
    {
        struct{
            DWORD TokenIsElevated;
        }te;
        DWORD dwReturnLength = 0;
        if (GetTokenInformation(hToken,(_TOKEN_INFORMATION_CLASS)20,&te,sizeof(te),&dwReturnLength))
        {
            if (dwReturnLength == sizeof(te))
                bIsElevated = te.TokenIsElevated;
        }
        CloseHandle(hToken);
    }
    return bIsElevated;
}

BOOL system::run_as_admin(LPCTSTR lpExeFile )
{
    SHELLEXECUTEINFO SEI = {sizeof(SHELLEXECUTEINFO)};
    SEI.lpVerb = _T("runas");
    SEI.lpFile = lpExeFile;
    SEI.nShow = SW_SHOWNORMAL;
    return ShellExecuteEx(&SEI);
}

tstring system::fetch_cmd_result(const tstring strParamt)
{
//     tstring strResult = _T("");
//     SECURITY_ATTRIBUTES sa;
//     HANDLE hRead,hWrite;
//     sa.nLength = sizeof(SECURITY_ATTRIBUTES);
//     sa.lpSecurityDescriptor = NULL;
//     sa.bInheritHandle = TRUE;
//     if (CreatePipe(&hRead,&hWrite,&sa,0) == FALSE) return FALSE;
// 
//     tstring strCommandFull = CLibX::File::GetSystemPath();
//     strCommandFull.append(_T("system32\\cmd.exe /c "));
//     strCommandFull.append(strParamt);
// 
//     STARTUPINFO si;
//     PROCESS_INFORMATION pi;
//     si.cb = sizeof(STARTUPINFO);
//     GetStartupInfo(&si);
//     si.hStdError = hWrite;
//     si.hStdOutput = hWrite;
//     si.wShowWindow = SW_HIDE;
//     si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
//     if (::CreateProcess(NULL,(lchar*)strCommandFull.c_str(),NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi) == FALSE)
//     {
//         CloseHandle(hWrite);
//         CloseHandle(hRead);
//         return FALSE;
//     }
// 
//     CloseHandle(hWrite);
// 
//     char buffer[4095] = {0};
//     DWORD bytesRead = 0;
//     while (true)
//     {
//         ReadFile(hRead,buffer,4095,&bytesRead,NULL);
//         if (bytesRead > 0 )
// #ifdef _UNICODE
//             strResult += CLibX::String::GBToUTF8(buffer);
// #else
//             strResult += buffer;
// #endif // _UNICODE
//         else
//             break;
//     }
// 
//     CloseHandle(hRead);

    //return strResult;

    return _T("");
}






}}