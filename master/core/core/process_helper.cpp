
#include "process_helper.h"
#include <windows.h>
#include <tchar.h>
#include <SHELLAPI.H>

#include <rdebug/log_helper.h>


using namespace klib::core;
using namespace klib::debug;
using namespace klib;

typedef 
    DWORD
    (WINAPI
    *GetModuleFileNameExA) (
    HANDLE hProcess,
    HMODULE hModule,
    LPSTR lpFilename,
    DWORD nSize
    );

typedef 
    DWORD
    (WINAPI
    *GetModuleFileNameExW) (
    HANDLE hProcess,
    HMODULE hModule,
    LPWSTR lpFilename,
    DWORD nSize
    );

typedef
    DWORD
    (WINAPI
    *GetProcessImageFileNameA) (
    HANDLE hProcess,
    LPSTR lpImageFileName,
    DWORD nSize
    );

typedef
    DWORD
    (WINAPI
    *GetProcessImageFileNameW) (
    HANDLE hProcess,
    LPWSTR lpImageFileName,
    DWORD nSize
    );

typedef 
    BOOL
    (WINAPI
    *WTSQueryUserToken)(
    ULONG SessionId,
    PHANDLE phToken
    );
    // Wtsapi32.dll
static api_fetcher<WTSQueryUserToken> WTSQueryUserToken_f(_T("Wtsapi32.dll"), "WTSQueryUserToken");

typedef
    BOOL
    (WINAPI
    *CreateEnvironmentBlock)(
    LPVOID  *lpEnvironment,
    HANDLE  hToken,
    BOOL    bInherit);
    // Userenv.dll
static api_fetcher<CreateEnvironmentBlock> CreateEnvironmentBlock_f(_T("Userenv.dll"), "CreateEnvironmentBlock");

typedef 
BOOL
    (WINAPI
    *DestroyEnvironmentBlock) (
    LPVOID  lpEnvironment);
    // Userenv.dll
static api_fetcher<DestroyEnvironmentBlock> DestroyEnvironmentBlock_f(_T("Userenv.dll"), "DestroyEnvironmentBlock");


typedef 
    DWORD
    (WINAPI
    *WTSGetActiveConsoleSessionId_)(
    VOID
    );
// WTSGetActiveConsoleSessionId_f
static api_fetcher<WTSGetActiveConsoleSessionId_> WTSGetActiveConsoleSessionId__f(_T("Kernel32.dll"), "WTSGetActiveConsoleSessionId");


#ifdef UNICODE
#define GetModuleFileNameEx  GetModuleFileNameExW
#define GetProcessImageFileName GetProcessImageFileNameW
#else
#define GetModuleFileNameEx  GetModuleFileNameExA
#define GetProcessImageFileName GetProcessImageFileNameA
#endif // !UNICODE


process_helper::process_helper(void)
{
}

process_helper::~process_helper(void)
{
}

BOOL process_helper::find_process(LPCTSTR pszProcessName)
{

    return TRUE;
}

BOOL process_helper::create_process(LPCTSTR pszExePath, LPTSTR pszCmdLine, HANDLE& hProcess)
{
    BOOL bCreateResult = FALSE;
    STARTUPINFO startInfo = {0};
    PROCESS_INFORMATION processInfo = {0};
    startInfo.cb = sizeof(startInfo);
    
    bCreateResult = ::CreateProcess(pszExePath, 
        pszCmdLine,
        NULL,
        NULL,
        FALSE,
        0, //CREATE_NO_WINDOW
        NULL,
        NULL,
        &startInfo,
        &processInfo
        );
    if (bCreateResult) {
        hProcess = processInfo.hProcess;
    }

    return bCreateResult;
}

BOOL process_helper::create_process(LPCTSTR pszExePath)
{
    TCHAR szCmdLine[] = {_T("")};
    HANDLE hProcess;

    if (create_process(pszExePath, szCmdLine, hProcess)) 
    {
        CloseHandle(hProcess);
        return TRUE;
    }
    else 
    {
        return FALSE;
    }
}

///< 关闭进程
BOOL process_helper::terminate_process(DWORD dwProcId)
{
    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);
    if (NULL == handle) {
        return FALSE;
    }

    return TerminateProcess(handle, 0);
}

BOOL process_helper::terminate_process(HANDLE handle)
{
    return TerminateProcess(handle, 0);
}

BOOL process_helper::shell_execute(LPCTSTR pszExePath, LPCTSTR pszCmd /*= _T("")*/)
{
    ::ShellExecute(NULL, _T("open"), pszExePath, pszCmd, _T(""), SW_SHOW);
    return TRUE;
}

BOOL process_helper::create_user_process(LPCTSTR pszExePath, LPTSTR pszCmd/* = _T("")*/)
{
    // 为了显示更加复杂的用户界面，我们需要从Session 0创建   
    // 一个进程，但是这个进程是运行在用户环境下。  
    // 我们可以使用CreateProcessAsUser实现这一功能。         
    BOOL bSuccess = FALSE;
    STARTUPINFO si = {0};
    // 进程信息   
    PROCESS_INFORMATION pi = {0};   
	
    // 创建用户Session环境     
    LPVOID lpEnvironment = NULL;
	
    // 复制令牌      
    HANDLE hDuplicatedToken = NULL; 

    WTSGetActiveConsoleSessionId_ WTSGetActiveConsoleSessionId_fun = (WTSGetActiveConsoleSessionId_)WTSGetActiveConsoleSessionId__f;
    if (NULL == WTSGetActiveConsoleSessionId_fun) 
    {
        return FALSE;
    }

    si.cb = sizeof(si);    
    // 获得当前Session ID     
    DWORD dwSessionID = WTSGetActiveConsoleSessionId_fun();          
    HANDLE hToken = NULL;

    // 获得当前Session的用户令牌
    WTSQueryUserToken WTSQueryUserToken_fun = (WTSQueryUserToken) WTSQueryUserToken_f;
    if (NULL == WTSQueryUserToken_fun) {
        return FALSE;
    }
    if (WTSQueryUserToken_fun(dwSessionID, &hToken) == FALSE)         
    {
        LOGGER()("error WTSQueryUserToken_fun...");
        goto Cleanup;
    }
    
    if (DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hDuplicatedToken) == FALSE)          
    {
        LOGGER()("error DuplicateTokenEx...");
        goto Cleanup;  
    }
    
    static CreateEnvironmentBlock CreateEnvironmentBlock_fun = (CreateEnvironmentBlock) CreateEnvironmentBlock_f;
    if (NULL == CreateEnvironmentBlock_fun) 
    {
        LOGGER()("error CreateEnvironmentBlock_fun...");
        goto Cleanup;
    }
    if (CreateEnvironmentBlock_fun(&lpEnvironment, hDuplicatedToken, FALSE) == FALSE)         
    {
        LOGGER()("error CreateEnvironmentBlock_fun...");
        goto Cleanup;
    }
    
    // 在复制的用户Session下执行应用程序，创建进程。          
    // 通过这个进程，就可以显示各种复杂的用户界面了 
    if (CreateProcessAsUser(hDuplicatedToken, 
        pszExePath, 
        pszCmd, 
        NULL, 
        NULL, 
        FALSE, 
        NORMAL_PRIORITY_CLASS |CREATE_NEW_CONSOLE |CREATE_UNICODE_ENVIRONMENT, 
        lpEnvironment, 
        NULL, 
        &si, 
        &pi) == FALSE)    
    {
        LOGGER() << "error CreateProcessAsUser Error : " 
            << GetLastError();
        goto Cleanup;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    bSuccess = TRUE;
    // 清理工作  
Cleanup:
    if (!bSuccess)         
    {
        
    }
    if (hToken != NULL)
        CloseHandle(hToken);
    if (hDuplicatedToken != NULL)
        CloseHandle(hDuplicatedToken);
    if (lpEnvironment != NULL)     
        DestroyEnvironmentBlock(lpEnvironment);

    return bSuccess;
}

BOOL process_helper::create_user_process(LPTSTR pszCmdline)
{
    // 为了显示更加复杂的用户界面，我们需要从Session 0创建   
    // 一个进程，但是这个进程是运行在用户环境下。  
    // 我们可以使用CreateProcessAsUser实现这一功能。         
    BOOL bSuccess = FALSE;
    STARTUPINFO si = {0};
    // 进程信息   
    PROCESS_INFORMATION pi = {0};   

    // 创建用户Session环境     
    LPVOID lpEnvironment = NULL;

    // 复制令牌      
    HANDLE hDuplicatedToken = NULL; 

    WTSGetActiveConsoleSessionId_ WTSGetActiveConsoleSessionId_fun = (WTSGetActiveConsoleSessionId_)WTSGetActiveConsoleSessionId__f;
    if (NULL == WTSGetActiveConsoleSessionId_fun) {
        return FALSE;
    }

    si.cb = sizeof(si);    
    // 获得当前Session ID     
    DWORD dwSessionID = WTSGetActiveConsoleSessionId_fun();          
    HANDLE hToken = NULL;

    // 获得当前Session的用户令牌
    WTSQueryUserToken WTSQueryUserToken_fun = (WTSQueryUserToken) WTSQueryUserToken_f;
    if (NULL == WTSQueryUserToken_fun) {
        return FALSE;
    }
    if (WTSQueryUserToken_fun(dwSessionID, &hToken) == FALSE)         
    {
        LOGGER()("error WTSQueryUserToken_fun...");
        goto Cleanup;
    }

    if (DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hDuplicatedToken) == FALSE)          
    {
        LOGGER()("error DuplicateTokenEx...");
        goto Cleanup;  
    }

    static CreateEnvironmentBlock CreateEnvironmentBlock_fun = (CreateEnvironmentBlock) CreateEnvironmentBlock_f;
    if (NULL == CreateEnvironmentBlock_fun) 
    {
        LOGGER()("error CreateEnvironmentBlock_fun...");
        return FALSE;
    }
    if (CreateEnvironmentBlock_fun(&lpEnvironment, hDuplicatedToken, FALSE) == FALSE)         
    {
        LOGGER()("error CreateEnvironmentBlock_fun...");
        goto Cleanup;
    }

    // 在复制的用户Session下执行应用程序，创建进程。          
    // 通过这个进程，就可以显示各种复杂的用户界面了 
    if (CreateProcessAsUser(hDuplicatedToken, 
        NULL, 
        pszCmdline,
        NULL, 
        NULL, 
        FALSE, 
        NORMAL_PRIORITY_CLASS |CREATE_NEW_CONSOLE |CREATE_UNICODE_ENVIRONMENT, 
        lpEnvironment, 
        NULL, 
        &si, 
        &pi) == FALSE)    
    {
        LOGGER() << "error CreateProcessAsUser Error :..."
            << GetLastError();
        goto Cleanup;       
    }         

    CloseHandle(pi.hProcess);         
    CloseHandle(pi.hThread);         
    bSuccess = TRUE;  
    // 清理工作  
Cleanup:          
    if (!bSuccess)         
    {             

    }
    if (hToken != NULL)             
        CloseHandle(hToken);         
    if (hDuplicatedToken != NULL)          
        CloseHandle(hDuplicatedToken);       
    if (lpEnvironment != NULL)             
        DestroyEnvironmentBlock(lpEnvironment);  

    return bSuccess;
}

DWORD process_helper::wait_process(HANDLE hProcess, DWORD dwMilliseconds)
{
    return ::WaitForSingleObject(hProcess, dwMilliseconds);
}

BOOL process_helper::get_process_exit_code(HANDLE hProcess, LPDWORD lpExitCode)
{
    return GetExitCodeProcess(hProcess, lpExitCode);
}

BOOL process_helper::enable_privilege(LPCTSTR lpPrivilegeName, BOOL bEnable)
{       
    TOKEN_PRIVILEGES Privileges;
    HANDLE hToken;
    BOOL bResult;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
        return FALSE;

    Privileges.PrivilegeCount = 1;
    Privileges.Privileges[0].Attributes = (bEnable) ? SE_PRIVILEGE_ENABLED : 0;

    if (!LookupPrivilegeValue(NULL, lpPrivilegeName,
        &Privileges.Privileges[0].Luid))
    {
        CloseHandle(hToken);
        return FALSE;
    }

    bResult = AdjustTokenPrivileges(hToken, FALSE, &Privileges, 0, NULL, NULL);

    CloseHandle(hToken);

    return bResult;
}

tstring process_helper::get_process_path(HANDLE hProcess)
{
#ifdef UNICODE
    static api_fetcher<GetModuleFileNameEx> f(_T("psapi.dll"), "GetModuleFileNameExW");
#else
    static api_fetcher<GetModuleFileNameEx> f(_T("psapi.dll"), "GetModuleFileNameExA");
#endif // !UNICODE

    GetModuleFileNameEx GetModuleFileNameEx_f = f;
    if (NULL == GetModuleFileNameEx_f) {
        return tstring();
    }

    TCHAR szName[2048] = {0};
    if (GetModuleFileNameEx_f(hProcess, 0, szName, _countof(szName)) > 0) 
    {
        return tstring(szName);
    }

    return tstring(_T(""));
}

tstring process_helper::get_process_path(DWORD dwProcId)
{
#ifdef UNICODE
    static api_fetcher<GetModuleFileNameEx> f(_T("psapi.dll"), "GetModuleFileNameExW");
#else
    static api_fetcher<GetModuleFileNameEx> f(_T("psapi.dll"), "GetModuleFileNameExA");
#endif // !UNICODE

    GetModuleFileNameEx GetModuleFileNameEx_f = f;
    if (NULL == GetModuleFileNameEx_f) {
        return tstring();
    }

    HANDLE hProcess; 
    TCHAR szName[2048] = {0};
    hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_QUERY_INFORMATION, TRUE, dwProcId); 
    if (hProcess > 0) 
    {
        if (GetModuleFileNameEx_f(hProcess, 0, szName, _countof(szName)) > 0) 
        {
            return tstring(szName);
        }
    }

    CloseHandle(hProcess);
    return tstring(_T(""));
}

tstring process_helper::get_curr_proc_path()
{
    TCHAR tmp_path[2*1024] = {0};
    DWORD dwRet = GetModuleFileName(GetModuleHandle(NULL), tmp_path, _countof(tmp_path));
    if (0 != dwRet) {
        return tmp_path;
    }

    return tstring(_T(""));
}

bool process_helper::get_process_commandline(HANDLE hProcess, tstring& cmdline)
{
    BOOL            bRet;
    DWORD           dwPos;
    LPBYTE          lpAddr;
    DWORD           dwRetLen;

    bRet = FALSE;  


    TCHAR pszCmdLine[2048] = {0};
    DWORD cchCmdLine = _countof(pszCmdLine);

    dwPos = 0;
    lpAddr = (LPBYTE)GetCommandLine;
Win7:
    if(lpAddr[dwPos] == 0xeb && lpAddr[dwPos + 1] == 0x05)
    {
        dwPos += 2;
        dwPos += 5;
Win8:  
        if(lpAddr[dwPos] == 0xff && lpAddr[dwPos + 1] == 0x25)  
        {  
            dwPos += 2;  
            lpAddr = *(LPBYTE*)(lpAddr + dwPos);  

            dwPos = 0;  
            lpAddr = *(LPBYTE*)lpAddr;  
WinXp:  
            if(lpAddr[dwPos] == 0xa1)  
            {  
                dwPos += 1;  
                lpAddr = *(LPBYTE*)(lpAddr + dwPos);  
                bRet = ReadProcessMemory(hProcess,  
                    lpAddr, 
                    &lpAddr,  
                    sizeof(LPBYTE),  
                    &dwRetLen);  
                if(bRet)  
                {  
                    bRet = ReadProcessMemory(hProcess,
                        lpAddr,
                        pszCmdLine,
                        cchCmdLine,
                        &dwRetLen);  
                }
            }
        }
        else
        {
            goto WinXp;
        }
    }
    else  
    {
        goto Win8;
    }

    if (bRet) {
        cmdline = pszCmdLine;
    }

    return bRet == TRUE;
}

//获取进程完整路径  
tstring process_helper::get_process_full_path(DWORD dwPID)
{
#ifdef UNICODE
    static api_fetcher<GetProcessImageFileName> f(_T("psapi.dll"), "GetProcessImageFileNameW");
#else
    static api_fetcher<GetProcessImageFileName> f(_T("psapi.dll"), "GetProcessImageFileNameA");
#endif // !UNICODE

    TCHAR       szImagePath[MAX_PATH];
    HANDLE      hProcess;

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, dwPID);
    if(!hProcess)
        return FALSE;

    GetProcessImageFileName GetProcessImageFileName_f = (GetProcessImageFileName) f;
    if (NULL == GetProcessImageFileName_f) {
        return tstring();
    }

    if(GetProcessImageFileName_f(hProcess, szImagePath, MAX_PATH))
    {
        CloseHandle(hProcess);
        return tstring();
    }

    tstring ts = dos_path_to_ntpath(szImagePath);
    
    CloseHandle(hProcess);

    return ts;
}

tstring process_helper::get_process_full_path(HANDLE hProcess)
{
#ifdef UNICODE
    static api_fetcher<GetProcessImageFileName> f(_T("psapi.dll"), "GetProcessImageFileNameW");
#else
    static api_fetcher<GetProcessImageFileName> f(_T("psapi.dll"), "GetProcessImageFileNameA");
#endif // !UNICODE

    TCHAR  szImagePath[MAX_PATH*2];
    
    GetProcessImageFileName GetProcessImageFileName_f = (GetProcessImageFileName) f;
    if (NULL == GetProcessImageFileName_f) {
        return tstring();
    }

    if(!GetProcessImageFileName_f(hProcess, szImagePath, _countof(szImagePath)))
    {
        return tstring();
    }

    tstring ts = dos_path_to_ntpath(szImagePath);

    return ts;
}

tstring process_helper::dos_path_to_ntpath(LPTSTR pszDosPath)  
{  
    TCHAR           szDriveStr[500];
    TCHAR           szDrive[3];
    TCHAR           szDevName[100];
    INT             cchDevName;
    INT             i;
    tstring         str_path;

    //检查参数  
    if(!pszDosPath)
        return FALSE;  

    //获取本地磁盘字符串  
    if(GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))  
    {  
        for(i = 0; szDriveStr[i]; i += 4)  
        {  
            if(!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))  
                continue;  

            szDrive[0] = szDriveStr[i];  
            szDrive[1] = szDriveStr[i + 1];  
            szDrive[2] = '\0';  
            if(!QueryDosDevice(szDrive, szDevName, 100))//查询 Dos 设备名  
                return FALSE;  

            cchDevName = lstrlen(szDevName);  
            if(_tcsnicmp(pszDosPath, szDevName, cchDevName) == 0)//命中  
            {
                str_path.append(szDrive);       //复制驱动器 
                str_path.append(pszDosPath + cchDevName);//复制路径  

                return str_path;  
            }             
        }  
    }

    return tstring();  
}  

