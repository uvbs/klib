
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
    init_pos(thelist);
    sytax_node* root = alloc_node();
    if (NULL == root) {
        return NULL;
    }

    sytax_node* parsed_node = NULL;
    const token* tk = NULL;
    while (tk = get_next_token())
    {
        if (tk->get_type() == token_name) 
        {
            parsed_node = parser_name_decl(tk);
        }
        else if (tk->get_type() == token_function) 
        {

        }


        root->siblings_.push_back(parsed_node);
    }

    return NULL;
}

void syntax_parser::init_pos(const token_list_type& thelist)
{
    token_list_ = & thelist;
    itr_ = token_list_->begin();
}

sytax_node* syntax_parser::alloc_node(node_type type)
{
    return new sytax_node(type);
}

const token* syntax_parser::get_next_token()
{
    if (itr_ != token_list_->end()) {
        const token* tk = & *itr_;
        itr_ ++;
        return tk;
    }

    return NULL;
}

sytax_node* syntax_parser::parser_name_decl(const token* tk)
{
    std::string name = tk->str();
    symbol_info info;
    bool ret;
    ret = symbols_mgr_->get_symbol(name, info);
    if (!ret) {
        info.token_ = *tk;
        symbols_mgr_->add_symbol(name, info);
    }

    return alloc_node();
}

sytax_node* syntax_parser::parser_function_decl(const token* tk)
{
    return alloc_node();
}

bool syntax_parser::is_match(const token* tk, token_type type)
{

    return false;
}