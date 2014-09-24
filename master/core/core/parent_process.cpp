#include "parent_process.h"

#include "process_enumer.h"
using namespace klib::core;

std::string parent_process::get_parent_process()
{
    process_enumer enumer;
    PROCESSENTRY32 info;

    DWORD dwParentId = 0;

    DWORD dwpid = GetCurrentProcessId();
    while (enumer.get_next_process_info(info))
    {
        if (dwpid == info.th32ProcessID) {
            dwParentId = info.th32ParentProcessID;
            break;
        }
    }

    enumer.reset();
    while (enumer.get_next_process_info(info))
    {
        if (dwParentId == info.th32ProcessID) {
            return info.szExeFile;
        }
    }

    return "";
}