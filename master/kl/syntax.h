#ifndef _kl_syntax_h_
#define _kl_syntax_h_

#include "token.h"
#include "symbols.h"
#include "node.h"

// syntax parser module
class syntax_parser
{
public:
    bool parser(const token_list_type& thelist) ;


private:

    symbols_mgr symbols_mgr_;
};



#endif // _kl_syntax_h_