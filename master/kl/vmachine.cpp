
#include "vmachine.h"
#include "lex.h"


void vmachine::test()
{
//     symbol_info str = "sdfsdf";
//     symbol_info fun = "md5";
//     fun.type_ = type_function;
// 
//     symbol_mgr_.add_symbol("sdfsdf", str);

    // symbol_mgr_.get_symbol();
}

bool vmachine::compile(const std::string& script)
{
    lex_parser parser(script.c_str(), script.size());

    token_list_type thelist;
    parser.parser_script(thelist);

    return true;
}

bool vmachine::run()
{
    symbol_info info;
    bool ret = symbol_mgr_.get_symbol("main", info);
    if (ret) {
        info.node_;
    }

    return true;
}


bool vmachine::invoke(const std::string& func, std::vector<variable>& var)
{

    return true;
}