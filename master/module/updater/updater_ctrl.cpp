#include "stdafx.h"
#include "updater_ctrl.h"

#include <core/process_enumer.h>
#include <core/process_helper.h>


using namespace klib;
using namespace klib::core;


updater_ctrl::updater_ctrl(void)
{
}

updater_ctrl::~updater_ctrl(void)
{
}

updater_ctrl* updater_ctrl::instance()
{
    static updater_ctrl _instance;
    return & _instance;
}

bool updater_ctrl::get_main_proc_info(proc_info& info)
{
    DWORD proc_id = GetCurrentProcessId();
    DWORD parent_proc_id = 0;
    tstring parent_proc_name;

    process_enumer proc_enumer;
    PROCESSENTRY32 proc_entry;
    while (proc_enumer.get_next_process_info(proc_entry))
    {
        if (proc_entry.th32ProcessID == proc_id)
        {
            parent_proc_id = proc_entry.th32ParentProcessID;
            break;
        }
    }
    if (parent_proc_id > 0) 
    {
        proc_enumer.reset();
        while (proc_enumer.get_next_process_info(proc_entry))
        {
            if (parent_proc_id == proc_entry.th32ProcessID) 
            {
                parent_proc_name = proc_entry.szExeFile;
                parent_proc_name = klib::io::path::extract_file_name(parent_proc_name);
                break;
            }
        }
    }

    if (parent_proc_id > 0) 
    {
        info.proc_id_   = parent_proc_id;
        info.proc_name_ = parent_proc_name;
    }
    
    return true;
}

void updater_ctrl::backup_main_exe(BOOL bBakup, tstring main_exe)
{
    tstring strAppPath;
    klib::io::path::get_app_path(strAppPath);

    tstring strMainExe = strAppPath + main_exe;
    tstring strBakupName = strMainExe + _T(".bak");

    // 备份主程序
    if (bBakup)
    {
        if (GetFileAttributes(strMainExe.c_str()) != -1) 
        {
            ::MoveFile(strMainExe.c_str(), strBakupName.c_str());
        }        
    }
    else
    {
        if (GetFileAttributes(strBakupName.c_str()) != -1) 
        {
            ::MoveFile(strBakupName.c_str(), strMainExe.c_str());
        }        
    }
}

void updater_ctrl::wait_for_main(const proc_info& info)
{
    // 找到父进程，并判断父进程是否退出了
    if (info.proc_id_ > 0) 
    {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|SYNCHRONIZE, 
            NULL, 
            info.proc_id_);

        if (hProcess) 
        {
            if (WAIT_TIMEOUT == WaitForSingleObject(hProcess, INFINITE))
            {
                //return -1;
            }
        }
    }
}

void updater_ctrl::exec_update_pkg(const tstring& app_path, const tstring& params)
{
    process_helper helper;

    // 启动更新程序
    HANDLE hProcess = 0;
    if (helper.create_process(app_path.c_str(), _T(""), hProcess))
    {
        if (WAIT_OBJECT_0 == helper.wait_process(hProcess, INFINITE))
        {
        }
    }
}