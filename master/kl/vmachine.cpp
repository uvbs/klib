
#include "vmachine.h"


bool vmachine::compile(const std::string& script)
{
    symbol_info str = "sdfsdf";
    symbol_info fun = "md5";
    fun.type_ = type_function;
    
    symbol_mgr_.add_symbol("sdfsdf", str);

    return true;
}

bool vmachine::run()
{
    return true;
}


bool vmachine::invoke(const std::string& func, variable& var)
{
    return true;
}