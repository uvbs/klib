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

};

