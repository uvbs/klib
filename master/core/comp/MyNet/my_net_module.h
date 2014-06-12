#ifndef _klib_my_net_module_h
#define _klib_my_net_module_h

#include <aom/iframework_imp.h>
#include "imynetdef.h"

using namespace klib::aom;

class my_net_module : 
    public module_imp,
    public imy_net
{
public:
    my_net_module(void);
    ~my_net_module(void);

    BEGIN_INTERFACE_ENTRY
        INTERFACE_ENTRY(IID_imy_net, imy_net)
    END_INTERFACE_ENTRY

public:
    virtual tcp_net_facade* create_tcp_facade();

};



#endif
