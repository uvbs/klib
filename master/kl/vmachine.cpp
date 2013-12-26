
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

    size_t len = 0;
    std::string var ;
    while (true)
    {
        parser_func_list thelist;
        parser.get_parser_funcs(thelist);

        token_list_type token_list_;

        do 
        {
            for (auto itr = thelist.begin(); itr != thelist.end(); ++itr)
            {
                len = itr->func();
                if (len > 0) 
                {
                    token tk;
                    tk.set_tks(parser.get_pos(), len);
                    tk.set_type(itr->type_);
                    tk.set_line_no(parser.get_line_no());
                    token_list_.push_back(tk);

                    parser.skip(len);
                    parser.skip_space();

                    var = tk.str();

                    break;
                }
            }

            if (len == 0) 
            {
                int error_x = 0;
                error_x = 1;
            }

        } while (true);
        

        len = parser.parser_name();
        if (len > 0) {
            var = parser.read(len);
        }

        len = parser.parser_int();
        std::string x = parser.read(len);

        variable int_v = parser.to_int(len);

        parser.skip(len);
        parser.skip_space();

        len = parser.parser_double();

        double dbl = parser.to_double(len);

        x = parser.read(len);


    }

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