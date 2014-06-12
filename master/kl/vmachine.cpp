
#include "vmachine.h"

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
    lex_parser lex_p(script.c_str(), script.size());
    syntax_parser sytax_p;

    parsser_error_info err_info;
    token_list_type thelist;
    if (!lex_p.parser_script(thelist, err_info))
    {
        // error handler
        lex_err_handler_ ? lex_err_handler_(err_info) : lex_err_handler_;
        return false;
    }
    
    sytax_p.init(&inter_env_);
    sytax_node* root_node = sytax_p.parser_tokens(thelist);
    if (NULL == root_node)
    {
        return false;
    }

    return true;
}

bool vmachine::run()
{
    symbol_info info;
    bool ret = inter_env_.get_global_symb()->get_symbol("main", info);
    if (ret) {
        info.node_;
    }

    return true;
}


bool vmachine::invoke(const std::string& func, std::vector<variable>& var)
{

    return true;
}