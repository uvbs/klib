#pragma once

#include <aom/iframework_imp.h>
#include "imynetdef.h"

using namespace klib::aom;

class CMyNetModule : 
    public module_imp,
    public IMyNet
{
public:
    CMyNetModule(void);
    ~CMyNetModule(void);

    BEGIN_INTERFACE_ENTRY
        INTERFACE_ENTRY(IID_IMyNet, IMyNet)
    END_INTERFACE_ENTRY

public:
    virtual tcpclient* CreateTcpClient();

};

