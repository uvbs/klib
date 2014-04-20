// updater.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "updater.h"

#include <Shellapi.h>

#include <istddef.h>
#include <klib_link.h>
#include <core/process_helper.h>
#include <core/app_instance.h>

#include "updater_ctrl.h"

using namespace klib;
using namespace klib::core;

#pragma comment(lib, "shell32")


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    updater_ctrl* ctrl_ = updater_ctrl::instance();
    klib::core::app_instance app_inst(_T("updater_myproc_%^&^$#@!sdkf"));
    if (app_inst.is_exist_prev_instance()) {
        return -1;
    }

    proc_info info;
    bool ret = ctrl_->get_main_proc_info(info);
    if (!ret) {
        printf("get main proc info exit");
        return -1;
    }

    ctrl_->wait_for_main(info);
    
    LPCWSTR lpGetCmdLine = GetCommandLineW();
    if (lpGetCmdLine) 
    {
        int pNumArgs;
        LPWSTR* pArgv = CommandLineToArgvW(lpGetCmdLine, &pNumArgs);
        if (pArgv && pNumArgs > 1) 
        {
            ctrl_->backup_main_exe(TRUE, info.proc_name_);
            ctrl_->exec_update_pkg(pArgv[1], _T(""));

            tstring app_path;
            klib::io::path::get_app_path(app_path);
            app_path += info.proc_name_;
            if (GetFileAttributes(app_path.c_str()) == -1) 
            {
                ctrl_->backup_main_exe(FALSE, info.proc_name_);
            }
        }
    }

	return 0;
}


