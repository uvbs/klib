// updater.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "updater.h"

#include <Shellapi.h>

#include <core/process_enumer.h>
#include <core/process_helper.h>
#include <istddef.h>
#include <klib_link.h>

using namespace klib;
using namespace klib::core;

#pragma comment(lib, "shell32")



// 睡眠时长 
#define   TIME_TO_SLEEEP        (500)

void BakupMainExe(BOOL bBakup, tstring main_exe)
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


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    DWORD proc_id = GetCurrentProcessId();
    DWORD parent_proc_id = 0;
    tstring proc_name;

	process_enumer proc_enumer;
    PROCESSENTRY32 proc_entry;
    while (proc_enumer.get_next_process_info(proc_entry))
    {
        if (proc_entry.th32ProcessID == proc_id)
        {
            parent_proc_id = proc_entry.th32ParentProcessID;
            proc_name = proc_entry.szExeFile;
        }
    }
    proc_name = klib::io::path::extract_file_name(proc_name);

    // 找到父进程，并判断父进程是否退出了
    if (parent_proc_id > 0) 
    {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|SYNCHRONIZE, NULL, parent_proc_id);
        if (hProcess) 
        {
            if (WAIT_TIMEOUT == WaitForSingleObject(hProcess, 30*1000))
            {
                //return -1;
            }
        }
    }
    
    Sleep(TIME_TO_SLEEEP);

    LPCWSTR lpGetCmdLine = GetCommandLineW();
    if (lpGetCmdLine) 
    {
        int pNumArgs;
        LPWSTR* pArgv = CommandLineToArgvW(lpGetCmdLine, &pNumArgs);
        if (pArgv && pNumArgs > 1) 
        {
            BakupMainExe(TRUE, proc_name);
            
            process_helper helper;
            HANDLE hProcess = 0;

            // 启动更新程序
            if (helper.create_process(pArgv[1], _T(""), hProcess))
            {
                if (WAIT_OBJECT_0 == helper.wait_process(hProcess, 120*1000))
                {

                }
            }

            //ShellExecute(NULL, _T("open"), pArgv[1], _T(""), _T(""), SW_SHOW);


            tstring strAppPath;
            klib::io::path::get_app_path(strAppPath);
            strAppPath += proc_name;
            if (GetFileAttributes(strAppPath.c_str()) == -1) 
            {
                BakupMainExe(FALSE, proc_name);
            }
        }
    }

	return (int) 0;
}


