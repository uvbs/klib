
#ifndef _kl_vmachine_h_
#define _kl_vmachine_h_

#include "kl.h"
#include "variable.h"
#include "symbols.h"
#include "arg_list.h"
#include "lex.h"
#include "syntax.h"

#include <string>
#include <functional>

typedef std::function<void(parsser_error_info&)> lex_error_handler;


// virtual machine
class vmachine
{
public:
    void test();
    bool compile(const std::string& script);
    bool run();

    /* func : º¯Êýµ÷ÓÃ */
    bool invoke(const std::string& func, std::vector<variable>& var);

protected:
    symbols_mgr         symbol_mgr_;
    lex_error_handler   lex_err_handler_;
};


#endif // _kl_vmachine_h_