
#ifndef _kl_vmachine_h_
#define _kl_vmachine_h_

#include "kl.h"
#include "variable.h"
#include "symbols.h"
#include "arg_list.h"

#include <string>


class vmachine
{
public:

    bool compile(const std::string& script);
    bool run();
    //bool 

    /* func : º¯Êýµ÷ÓÃ */
    bool invoke(const std::string& func, variable& var);

protected:
    symbols_mgr  symbol_mgr_;

};


#endif // _kl_vmachine_h_