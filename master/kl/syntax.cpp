
#include "syntax.h"

syntax_parser::syntax_parser() :
    inter_env_(nullptr)
{

}

bool syntax_parser::init(inter_env* env)
{
    if (nullptr == inter_env_) {
        return false;
    }
    inter_env_ = env;
    return true;
}

sytax_node* syntax_parser::parser_tokens(const token_list_type& thelist) 
{
    init_pos(thelist);
    sytax_node* root = alloc_node();
    if (nullptr == root) {
        return nullptr;
    }

    sytax_node* parsed_node = nullptr;
    const token* tk = nullptr;
    while (tk = get_next_token())
    {
        if (tk->get_type() == token_name) 
        {
            parsed_node = parser_name_def(tk);
        }
        else if (tk->get_type() == token_var) 
        {
            parsed_node = parser_var_def(tk);
        }
        else if (tk->get_type() == token_function) 
        {
            parsed_node = parser_function_def(tk);
        }
        else if (tk->get_type() == token_array) {
            parsed_node = parser_array_def(tk);
        }

        if (parsed_node)
        {
            root->siblings_.push_back(parsed_node);
        }
        else {
            // printf error
            break;
        }
    }

    return nullptr;
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

    return nullptr;
}

const token* syntax_parser::get_cur_token()
{
    if (itr_ != token_list_->end()) {
        return (&*itr_);
    }
    return nullptr;
}

sytax_node* syntax_parser::parser_name_def(const token* tk)
{
    std::string name = tk->str();
    symbol_info info;
    bool ret;
    ret = inter_env_->get_global_symb()->get_symbol(name, info);
    if (!ret) {
        info.token_ = *tk;
        inter_env_->get_global_symb()->add_symbol(name, info);
    }
    else {
        info.token_ = *tk;
        inter_env_->get_global_symb()->set_symbol(name, info);
    }
    return alloc_node();
}

sytax_node* syntax_parser::parser_var_def(const token* tk)
{
    const token* next_tk = get_next_token();
    if (nullptr == next_tk) {
        return nullptr;
    }

    if (next_tk->get_type() == token_name) {
        return parser_name_def(next_tk);
    }

    return nullptr;
}

sytax_node* syntax_parser::parser_function_def(const token* tk)
{
    return alloc_node();
}

sytax_node* syntax_parser::parser_array_def(const token* tk)
{
    return alloc_node();
}

bool syntax_parser::is_match(const token* tk, token_type type)
{
    return (tk->get_type() == type);
}