
#include "syntax.h"

syntax_parser::syntax_parser() :
    symbols_mgr_(NULL)
{

}

bool syntax_parser::init(symbols_mgr* mgr)
{
    symbols_mgr_ = mgr;
    return true;
}

sytax_node* syntax_parser::parser_tokens(const token_list_type& thelist) 
{
    sytax_node* root = alloc_node();
    if (NULL == root) {
        return NULL;
    }

    auto itr = thelist.begin();
    for (; itr != thelist.end(); ++ itr)
    {
        const token* tok = &*itr;

        // @todo Ö´ÐÐÓï·¨½âÎö

        if (tok->get_type() == token_name) 
        {
            std::string name = tok->str();
            symbol_info info;
            bool ret;
            ret = symbols_mgr_->get_symbol(name, info);
            if (!ret) {
                info.token_ = *tok;
                symbols_mgr_->add_symbol(name, info);
            }
        }
        else if (tok->get_type() == token_assign) 
        {

        }

    }

    return NULL;
}

sytax_node* syntax_parser::alloc_node()
{
    return new sytax_node;
}