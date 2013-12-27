
#include "syntax.h"


bool syntax_parser::init(symbols_mgr* mgr)
{
    symbols_mgr_ = mgr;
    return true;
}

bool syntax_parser::parser(const token_list_type& thelist) 
{
    auto itr = thelist.begin();
    for (; itr != thelist.end(); ++ itr)
    {
        const token& tok = *itr;

        // @todo Ö´ĞĞÓï·¨½âÎö


    }

    return true;
}