#pragma once

#ifdef _MSC_VER
#include <string>
#include <tchar.h>
#include <Windows.h>
#else
#error "not support yet"
#endif

namespace klib
{

using namespace std;


// 设置开机启动
bool SetAppRunBoot(const TCHAR* szAppName, const TCHAR* szCommandLine, size_t nLen, bool bSetBoot/* = true*/) ;
bool IsSetAppRunBoot(const TCHAR* szAppName);

// 设置debug权限
BOOL DebugPrivilegeEnable(BOOL Enable=true);

// 添加内存地址保护
BOOL   AddProtect(void *addr, int memSize);
BOOL   AddProtect(HANDLE processHandle, void *addr, unsigned long &proctect);
// 去掉内存地址保护，使内存区域可读，可写，可执行
BOOL   RemoveProtect(void *addr, int memSize);
BOOL   RemoveProtect(HANDLE processHandle, void *addr, unsigned long &oldProctect);

// 远程注入函数,proHandle远程进程句柄
void* InjectProcess(const char *dllName, HANDLE proHandle);

}