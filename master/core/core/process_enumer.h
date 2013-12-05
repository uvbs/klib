#pragma once

#include <wtypes.h>
#include <windows.h>
#include <Tlhelp32.h>
#include <functional>

namespace klib{
namespace core {


//----------------------------------------------------------------------
// 进程枚举类
class  process_enumer
{
public:
    process_enumer(void);
    ~process_enumer(void);

public:
    BOOL reset();
    BOOL get_next_process_info(PROCESSENTRY32 &info);
    VOID for_each(std::function<void(PROCESSENTRY32 &)> fun);

protected:
    HANDLE m_hProcessSnap;
    BOOL   m_bFirst;
};


}}
