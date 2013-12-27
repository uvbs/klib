#ifndef _kl_syntax_h_
#define _kl_syntax_h_

#include "token.h"
#include "symbols.h"
#include "node.h"

// syntax parser module
class syntax_parser
{
public:
    syntax_parser() ;

    // init syntax parser
    bool init(symbols_mgr* mgr);

    // parser tokens and return the sytax tree
    sytax_node* parser_tokens(const token_list_type& thelist) ;

protected:
    sytax_node* alloc_node();

private:
    symbols_mgr* symbols_mgr_;
};



#endif // _kl_syntax_h_