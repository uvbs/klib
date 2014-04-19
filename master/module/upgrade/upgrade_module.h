#pragma once

#include "../include/upgrade_interface.h"


class upgrade_module : 
    public module_imp,
    public upgrade_i
{
public:
    upgrade_module(void);
    ~upgrade_module(void);

public:
    BEGIN_INTERFACE_ENTRY
        INTERFACE_ENTRY(IID_UPGRADE_I, upgrade_i)
    END_INTERFACE_ENTRY

protected:
    bool set_cur_ver_info(const upgrade_info& info);
    bool get_new_ver_info(version_info& info);
    bool down_new_file(const version_info& new_ver_info, const std::string& new_path);
    virtual bool do_upgrade(const tstring& upgrade_pkg,
        const std::string& main_exe) ;
    bool auto_upgrade();

protected:
    upgrade_info upgrade_info_;
};

