#pragma once

#include <istddef.h>
using namespace klib;


// Ë¯ÃßÊ±³¤ 
#define   TIME_TO_SLEEEP        (500)



struct proc_info
{
    tstring proc_name_;
    DWORD   proc_id_;
};

class updater_ctrl
{
    updater_ctrl(void);
public:
    ~updater_ctrl(void);

    static updater_ctrl* instance();

public:
    bool get_main_proc_info(proc_info& info);
    void backup_main_exe(BOOL bBakup, tstring main_exe);
    void wait_for_main(const proc_info& info);

    void exec_update_pkg(const tstring& app_path, const tstring& params);

protected:

};

