#pragma once

#include "../istddef.h"
#include "../inttype.h"
#include "api_fetcher.h"

#include <windows.h>
#include <tchar.h>

namespace klib{
namespace core{

//----------------------------------------------------------------------
// 进程相关操作类
class process_helper
{
public:
    process_helper(void);
    ~process_helper(void);

    ///< 查找进程
    static BOOL find_process(LPCTSTR pszProcessName);

    ///< 创建进程
    static BOOL create_process(LPCTSTR pszExePath, LPTSTR pszCmdLine, HANDLE& hProcess);
    
    ///< 创建进程
    static BOOL create_process(LPCTSTR pszExePath);
    
    ///< 关闭进程
    static BOOL terminate_process(DWORD dwProcId);

    ///< 关闭进程
    static BOOL terminate_process(HANDLE handle);

    static BOOL shell_execute(LPCTSTR pszExePath, LPCTSTR pszCmd = _T(""));

    ///< 以用户权限创建进程
    static BOOL create_user_process(LPCTSTR pszExePath, LPTSTR pszCmd = _T(""));

    static BOOL create_user_process(LPTSTR pszCmdline);

    ///< 等待进程结束
    static DWORD wait_process(HANDLE hProcess, DWORD dwMilliseconds);

    ///< 获得退出的代码
    static BOOL get_process_exit_code(HANDLE hProcess, LPDWORD lpExitCode);

    ///< 设置权限 
    static BOOL enable_privilege(LPCTSTR pszprivilege, BOOL beable) ;

    ///< 获取进程路径
    static tstring get_process_path(HANDLE handle);

    ///< 获取进程路径
    static tstring get_process_path(DWORD dwProcId);

    ///< 获取当前进程路径
    static tstring get_curr_proc_path();

    ///< 获取进程的命令行
    static bool get_process_commandline(HANDLE handle, tstring&);
    
    // 获取进程完整路径  
    static tstring get_process_full_path(DWORD dwPID);

    // 获取进程的完整路径
    static tstring get_process_full_path(HANDLE handle);

protected:
    static tstring dos_path_to_ntpath(LPTSTR pszDosPath);
};

}}