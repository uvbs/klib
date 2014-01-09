#include "cpu.h"
#include <Windows.h>

namespace klib {
namespace sys {



///< 文件时间转换为世界时间
UINT64 FileTimeToUtc(const FILETIME* ftime) 
{ 
    LARGE_INTEGER li; 
    _ASSERT(ftime); 
    li.LowPart = ftime->dwLowDateTime; 
    li.HighPart = ftime->dwHighDateTime; 
    return li.QuadPart; 
} 


UINT32 cpu::get_cpu_number()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return (UINT32)info.dwNumberOfProcessors;
}

INT32 cpu::get_cpu_usage() 
{ 
    //cpu数量 
    int cupCnt= -1; 

    //上一次的时间 
    static UINT64 lastTime = 0; 
    static UINT64 lastSysTime = 0; 

    FILETIME now; 
    FILETIME creatTime; 
    FILETIME exitTime; 
    FILETIME kernelTime; 
    FILETIME userTime; 
    UINT64 sytTime; 
    UINT64 time; 
    UINT64 sysTimeDelta; 
    UINT64 timeDelta; 

    int cpu = -1; 
    if (cupCnt == -1) 
    { 
        cupCnt = get_cpu_number(); 
    } 

    GetSystemTimeAsFileTime(&now); 

    if (!GetProcessTimes(GetCurrentProcess(), &creatTime, &exitTime, 
        &kernelTime, &userTime)) 
    { 
        // We don't assert here because in some cases (such as in the Task  Manager) 
        // we may call this function on a process that has just exited but we have 
        // not yet received the notification. 
        return -1; 
    } 

    sytTime = (FileTimeToUtc(&kernelTime) + FileTimeToUtc(&userTime)) / cupCnt; 

    time = FileTimeToUtc(&now); 

    if ((lastSysTime == 0) || (lastTime == 0)) 
    { 
        // First call, just set the last values. 
        lastSysTime = sytTime; 
        lastTime = time; 
        return -1; 
    } 

    sysTimeDelta = sytTime - lastSysTime; 
    timeDelta = time - lastTime; 

    if (timeDelta == 0) 
    {
        return -1; 
    }

    // We add timeDelta / 2 so the result is rounded. 
    cpu = (int)((sysTimeDelta * 100 + timeDelta / 2) / timeDelta); 
    lastSysTime = sytTime; 
    lastTime = time; 

    return cpu; 
} 





}}