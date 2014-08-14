#ifndef _klib_sys_h
#define _klib_sys_h


#include "../istddef.h"
#include "../inttype.h"

#ifdef _MSC_VER
#include <string>
#include <tchar.h>
#include <Windows.h>
#else
#error "not support yet"
#endif

namespace klib {
namespace sys {


using namespace std;
using namespace klib;


class system
{
public:
    // 设置开机启动
    static bool SetAppRunBoot(const TCHAR* szAppName, const TCHAR* szCommandLine, size_t nLen, bool bSetBoot/* = true*/) ;
    static bool IsSetAppRunBoot(const TCHAR* szAppName);

    // 设置debug权限
    static BOOL DebugPrivilegeEnable(BOOL Enable=true);

    // 添加内存地址保护
    static BOOL   AddProtect(void *addr, int memSize);
    static BOOL   AddProtect(HANDLE processHandle, void *addr, unsigned long &proctect);
    // 去掉内存地址保护，使内存区域可读，可写，可执行
    static BOOL   RemoveProtect(void *addr, int memSize);
    static BOOL   RemoveProtect(HANDLE processHandle, void *addr, unsigned long &oldProctect);

    // 远程注入函数,proHandle远程进程句柄
    static void* InjectProcess(const char *dllName, HANDLE proHandle);

    // 判断是否是管理员
    static BOOL is_admin();

    // 以管理员权限运行
    static BOOL run_as_admin(LPCTSTR lpExeFile );

    static tstring fetch_cmd_result(const tstring strParamt);

    static void restart_system();
};




}}

#endif
