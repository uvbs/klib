
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
    while (true)
    {
        len = parser.parser_digital();
        std::string x = parser.read(len);

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
    return true;
}


bool vmachine::invoke(const std::string& func, variable& var)
{
    return true;
}