#pragma once


#include "../../dll/include/lock.h"
#include "../../dll/src/addr_mgr.h"

class mem_lib
{
    mem_lib(void);
public:
    ~mem_lib(void);

    static mem_lib* instance();



protected:
    static mem_lib* m_instance;
    addr_mgr*       m_addr_mgr;

};

