
#include "lex.h"

#include <locale>


bool lex_parser::parser_script(token_list_type& token_lst, parsser_error_info& err_info)
{
    parser_func_list thelist;
    this->get_parser_funcs(thelist);

    parser_result p_ret;
    std::string var ;
    do 
    {
        for (auto itr = thelist.begin(); itr != thelist.end(); ++itr)
        {
            p_ret = itr->func_();
            if (p_ret.length() > 0) 
            {
                token tk;
                tk.set_tks(this->get_pos(), p_ret.length());
                tk.set_type(p_ret.type() == token_none ? itr->type_ : p_ret.type());
                tk.set_line_no(this->get_line_no());
                token_lst.push_back(tk);

                this->skip(p_ret.length());
                this->skip_space();
                break;
            }
        }

        if (p_ret.length() == 0) 
        {
            err_info.line_no_ = get_line_no();
            err_info.err_pos_ = get_pos();
            err_info.err_msg_ = "unreconise token found!!!";
            return false;
        }

        if (is_finished()) {
            break;
        }

    } while (true);    

    return true;
}

bool lex_parser::is_finished() const 
{
    return (cur_ptr_ == script_buff_ + script_len_);
}

parser_result lex_parser::parser_comment()
{
    auto pos = cur_ptr_;
    if (*pos != '/') 
    {
        return 0;
    }

    ++ pos;
    if (*pos == '*') 
    {
        size_t len = this->parser_until(pos, "*/", 2);
        if (len > 0) 
        {
            return len + 2 + 2;
        }
    }
    else if (*pos == '/') 
    {
        size_t len = this->parser_until(pos, "\n", 1);
        if (len > 0) 
        {
            return 2 + len;
        }

        len = this->parser_until(pos, "\r\n", 2);
        if (len > 0) 
        {
            return 2 + len;
        }
    }

    return 0;    
}

bool lex_parser::get_parser_funcs(parser_func_list& thelist)
{
    parser_func_info funcs[] =
    {
        parser_func_info(token_comment,     std::bind(&lex_parser::parser_comment, this)),
        parser_func_info(token_name,        std::bind(&lex_parser::parser_name, this)),
        parser_func_info(token_double,      std::bind(&lex_parser::parser_double, this)),
        parser_func_info(token_string,      std::bind(&lex_parser::parser_string, this)),
        parser_func_info(token_logic,       std::bind(&lex_parser::parser_logic, this)),
        parser_func_info(token_bracket,     std::bind(&lex_parser::parser_bracket, this)),
        parser_func_info(token_keyword,     std::bind(&lex_parser::parser_keyword, this)),
        parser_func_info(token_int,         std::bind(&lex_parser::parser_int, this)),
        parser_func_info(token_operator,    std::bind(&lex_parser::parser_operator, this)),
        parser_func_info(token_assign,      std::bind(&lex_parser::parser_assign, this)),
        parser_func_info(token_line,        std::bind(&lex_parser::parser_line, this)),
        parser_func_info(token_statement,   std::bind(&lex_parser::parser_statement, this)),
    };

    for (auto index=0; index < _countof(funcs); ++index)
    {
        thelist.push_back(funcs[index]);
    }

    return true;
}

bool lex_parser::init_keyword_list()
{
    keyword_info infos[] = 
    {
        keyword_info("class", token_class),
        keyword_info("function", token_function),
        keyword_info("var", token_var),
        keyword_info("return", token_return),
        keyword_info("if", token_if),        
        keyword_info("this", token_this),
        keyword_info("while", token_while),
        keyword_info("do", token_do),
        keyword_info("public", token_public),
        keyword_info("private", token_private),
        keyword_info("protect", token_protect),
    };

    for (auto index=0; index < _countof(infos); ++index) 
    {
        keyword_list_.insert(infos[index]);
    }
    return true;
}

parser_result lex_parser::parser_int()
{
    return this->parser_(cur_ptr_, is_digital);
}

parser_result lex_parser::parser_name()               // 解析是否是名字
{
    auto pos = cur_ptr_;
    if (*pos != '$') {
        return 0;
    }

    ++ pos;
    if (is_letter(*pos) || is_underline(*pos)) 
    {
        return this->parser_(pos + 1, is_digi_letter_underline) + 2;
    }
    return 0;
}

parser_result lex_parser::parser_string()             // 解析是否是字符串
{
    auto pos = cur_ptr_;
    if (*pos != '"') {
        return 0;
    }

    ++ pos;
    if (pos >= end_ptr_) {
        return 0;
    }

    bool find_end = false;
    while (pos < end_ptr_)
    {
        if (*pos == '"' && *(pos - 1) != '\\') {
            find_end = true;
            break;
        }

        ++ pos;
    }

    return parser_result(pos - cur_ptr_ + 1, token_string);
}

parser_result lex_parser::parser_keyword()           // 判断是否是函数
{
    auto pos = cur_ptr_;
    if (!is_letter(*pos) && !is_underline(*pos)) 
    {
        return 0;
    }

    size_t len_key = this->parser_(pos + 1, is_digi_letter_underline);
    ++ len_key;

    keyword_info kinfo;
    kinfo.init(pos, len_key, token_none);
    auto itr = keyword_list_.find(kinfo);
    if (itr != keyword_list_.end()) 
    {
        return parser_result(len_key, itr->type());
    }
    else 
    {
        return parser_result(len_key, token_label);
    }

    return 0;
}

parser_result lex_parser::parser_double()             // 解析是否是浮点数
{
    parser_result int_ret = this->parser_int();
    const char_type* pos = cur_ptr_ + int_ret.length();

    if (0 == this->parser_(pos, '.')) {
        return 0;
    }
    size_t len_ext = this->parser_(pos + 1, is_digital);
    return len_ext + int_ret.length() + 1;
}

parser_result lex_parser::parser_logic()
{
    auto pos = cur_ptr_;

    if (*pos == '<')
    {
        if (*(pos + 1) == '=') {
            return parser_result(2, token_logic_less_eq);
        }
        return parser_result(1, token_logic_less);
    }
    else if (*pos == '>')
    {
        if (*(pos + 1) == '=') {
            return parser_result(2, token_logic_greater_eq);
        }
        return parser_result(1, token_logic_greater);
    }
    else if (*pos == '!')
    {
        if (*(pos + 1) == '=') {
            return parser_result(2, token_logic_not_equal);
        }
        return parser_result(1, token_logic_not);
    }
    else if (*pos == '=') {
        if (*(pos + 1) == '=') {
            return parser_result(2, token_logic_equal);
        }
    }
    else if (*pos == '|') {
        if (*(pos + 1) == '|') {
            return parser_result(2, token_logic_or);
        }
    }
    else if (*pos == '&') {
        if (*(pos + 1) == '&') {
            return parser_result(2, token_logic_and);
        }
    }

    return 0;
}

parser_result lex_parser::parser_operator()           // 解析运算符
{
    auto pos = cur_ptr_;
    bool find_op = false;

    if (*pos == '+') 
    {
        if (*(pos + 1) == '=') {
            return parser_result(2, token_operator_add_assign);
        }
        return parser_result(1, token_operator_add);
    }
    else if (*pos == '-') 
    {
        if (*(pos + 1) == '=') {
            return parser_result(2, token_operator_minus_assign);
        }
        return parser_result(1, token_operator_minus);
    }
    else if (*pos == '*') 
    {
        if (*(pos + 1) == '=') {
            return parser_result(2, token_operator_multi_assign);
        }
        return parser_result(1, token_operator_multi);
    }
    else if (*pos == '/') 
    {
        if (*(pos + 1) == '=') {
            return parser_result(2, token_operator_divide_assign);
        }
        return parser_result(1, token_operator_divide);
    }
    else if (*pos == '%') {
        if (*(pos + 1) == '=') {
            return parser_result(2, token_operator_mod_assign);
        }
        return parser_result(1, token_operator_mod);
    }

    return 0;
}

parser_result lex_parser::parser_bracket()            // 解析括号 () {}[]
{
    auto pos = cur_ptr_;
    if (*pos == '(') {
        return parser_result(1, token_left_parentheses);
    }
    else if (*pos == ')') {
        return parser_result(1, token_right_parentheses);
    }
    else if (*pos == '{') {
        return parser_result(1, token_left_brace);
    }
    else if (*pos == '}') {
        return parser_result(1, token_right_brace);
    }
    else if (*pos == '[') {
        return parser_result(1, token_left_bracket);
    }
    else if (*pos == ']') {
        return parser_result(1, token_right_bracket);
    }

    return 0;
}

parser_result lex_parser::parser_assign()             // 解析赋值符
{
    if (*cur_ptr_ == '=') {
        return 1;
    }
    return 0;
}

parser_result lex_parser::parser_line()
{
    auto pos = cur_ptr_;
    if (this->parser_(pos, '\n')) 
    {
        ++ line_no_;
        return 1;
    }

    if (this->parser_(pos, '\r') && 
        this->parser_(pos + 1, '\n')) 
    {
        ++ line_no_;
        return 2;
    }

    return 0;
}

parser_result lex_parser::parser_statement()
{
    if (*cur_ptr_ == ';') {
        return parser_result(1, token_semicolon);
    }

    return 0;
}

size_t lex_parser::parser_(const char_type* pos, const parser_callback& call)
{
    size_t parsed_count = 0;
    while(call(*pos))
    {
        pos ++;
        parsed_count ++;
    }

    return parsed_count;
}

size_t lex_parser::parser_(const char_type* pos, char_type ch)
{
    if (*pos == ch) {
        return 1;
    }
    return 0;
}

size_t lex_parser::parser_(const char_type* pos, char_type* str, size_t len)
{
    if (strncmp(pos, str, len) == 0) {
        return len;
    }
    return 0;
}

size_t lex_parser::parser_until(const char_type* pos, char_type ch)
{
    size_t parsed_count = 0;
    while(*pos != ch)
    {
        pos ++;
        parsed_count ++;
    }

    return parsed_count;
}

size_t lex_parser::parser_until(const char_type* pos, char_type* str, size_t len)
{
    size_t parsed_count = 0;
    while(strncmp(pos, str, len) != 0)
    {
        pos ++;
        parsed_count ++;
    }

    return parsed_count;
}

size_t lex_parser::parser_until(const char_type* pos, const parser_callback& call)
{
    size_t parsed_count = 0;
    while(!call(*pos))
    {
        pos ++;
        parsed_count ++;
    }

    return parsed_count;
}

size_t lex_parser::skip_space()
{
    size_t len = this->parser_(cur_ptr_, std::bind(&lex_parser::is_space, this, std::placeholders::_1));
    cur_ptr_ += len;
    return len;
}

size_t lex_parser::skip(size_t len)
{
    size_t len_to_skip = len;
    while (len_to_skip > 0 && *cur_ptr_)
    {
        if (*cur_ptr_ == '\n') 
        {
            ++ line_no_;
        }

        ++ cur_ptr_;
        -- len_to_skip;
    }
    return (len - len_to_skip);
}

std::string lex_parser::read(size_t len)
{
    return std::string(cur_ptr_, len);
}

double lex_parser::to_double(size_t len)
{
    char* ptmp = 0;
    std::string s = read(len);
    return ::strtod(s.c_str(), &ptmp);
}

int lex_parser::to_int(size_t len)
{
    char* ptmp = 0;
    std::string s = read(len);
    return ::strtol(s.c_str(), &ptmp, 10);
}

bool lex_parser::is_digital(char_type ch)
{
    return ::isdigit(ch) != 0;
}

bool lex_parser::is_letter(char_type ch)
{
    if (ch >= 'a' && ch <= 'z') {
        return true;
    }

    if (ch >= 'A' && ch <= 'Z') {
        return true;
    }

    return false;
}

bool lex_parser::is_digi_letter(char_type ch)
{
    if (is_letter(ch)) {
        return true;
    }

    return is_digital(ch);
}

bool lex_parser::is_underline(char_type ch)
{
    return ch == '_';
}

bool lex_parser::is_digi_letter_underline(char_type ch)
{
    if (is_digi_letter(ch)) {
        return true;
    }

    return ch == '_';
}

bool lex_parser::is_space(char_type ch)
{
    if (ch == '\n') {
        ++ line_no_;
    }

    if (isspace(ch)) {
        return true;
    }

    return false;
}