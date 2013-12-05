//#include "StdAfx.h"
#include "process_helper.h"
//#include <windows.h>
#include <tchar.h>

using namespace  klib::core;

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

BOOL process_helper::shell_execute(LPCTSTR pszExePath)
{
    ::ShellExecute(NULL, _T("open"), pszExePath, _T(""), _T(""), SW_SHOW);
    return TRUE;
}

DWORD process_helper::wait_process(HANDLE hProcess, DWORD dwMilliseconds)
{
    return ::WaitForSingleObject(hProcess, dwMilliseconds);
}

BOOL process_helper::get_process_exit_code(HANDLE hProcess, LPDWORD lpExitCode)
{
    return GetExitCodeProcess(hProcess, lpExitCode);
}