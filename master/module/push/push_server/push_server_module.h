#pragma once

#include "../../include/push_interface.h"

class push_server_module : 
    public module_imp,
    public push_server_i
{
public:
    push_server_module(void);
    ~push_server_module(void);

public:
    BEGIN_INTERFACE_ENTRY
        INTERFACE_ENTRY(IID_PUSH_CLIENT, push_server_i)
    END_INTERFACE_ENTRY

protected:

};

