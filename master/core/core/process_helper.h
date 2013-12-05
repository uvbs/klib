#pragma once

#include <windows.h>

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

    static BOOL shell_execute(LPCTSTR pszExePath);

    ///< 等待进程结束
    static DWORD wait_process(HANDLE hProcess, DWORD dwMilliseconds);

    ///< 获得退出的代码
    static BOOL get_process_exit_code(HANDLE hProcess, LPDWORD lpExitCode);
};

}}