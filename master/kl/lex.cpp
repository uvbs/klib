
#include "lex.h"

#include <locale>


bool lex_parser::get_parser_funcs(parser_func_list& thelist)
{
    parser_func_info fx;

    fx.type_ = token_comment;
    fx.func = std::bind(&lex_parser::parser_comment, this);
    thelist.push_back(fx);

    fx.type_ = token_name;
    fx.func = std::bind(&lex_parser::parser_name, this);
    thelist.push_back(fx);

    fx.type_ = token_double;
    fx.func = std::bind(&lex_parser::parser_double, this);
    thelist.push_back(fx);

    fx.type_ = token_function;
    fx.func = std::bind(&lex_parser::parser_function, this);
    thelist.push_back(fx);

    fx.type_ = token_int;
    fx.func = std::bind(&lex_parser::parser_int, this);
    thelist.push_back(fx);

    fx.type_ = token_class;
    fx.func = std::bind(&lex_parser::parser_class, this);
    thelist.push_back(fx);

    fx.type_ = token_operator;
    fx.func = std::bind(&lex_parser::parser_operator, this);
    thelist.push_back(fx);
        
    fx.type_ = token_assign;
    fx.func = std::bind(&lex_parser::parser_assign, this);
    thelist.push_back(fx);

    fx.type_ = token_line;
    fx.func = std::bind(&lex_parser::parser_line, this);
    thelist.push_back(fx);

    fx.type_ = token_statement;
    fx.func = std::bind(&lex_parser::parser_statement, this);
    thelist.push_back(fx);

    return true;
}

size_t lex_parser::parser_comment()
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

size_t lex_parser::parser_int()
{
    return this->parser_(cur_ptr_, is_digital);
}

size_t lex_parser::parser_name()               // 解析是否是名字
{
    auto pos = cur_ptr_;
    if (*pos != '$') {
        return 0;
    }

    ++ pos;
    if (is_letter(*pos)) 
    {
        return this->parser_(pos + 1, is_digi_letter) + 2;
    }
    return 0;
}

size_t lex_parser::parser_double()             // 解析是否是浮点数
{
    size_t len_main = this->parser_int();
    const char_type* pos = cur_ptr_ + len_main;

    if (0 == this->parser_(pos, '.')) {
        return 0;
    }
    size_t len_ext = this->parser_(pos + 1, is_digital);
    return len_ext + len_main + 1;
}

size_t lex_parser::parser_function()           // 判断是否是函数
{
    auto pos = cur_ptr_;
    size_t len_func =  this->parser_(pos, "function ", 9);
    if (len_func > 0) 
    {
        return len_func - 1;
    }
    return 0;
}

size_t lex_parser::parser_class()              // 判断是否是一个类
{

    return 0;
}

size_t lex_parser::parser_logic()
{
    if (*cur_ptr_ == '<' ||
        *cur_ptr_ == '>' 
        ) 
    {
        return 1;
    }

    return 0;
}

size_t lex_parser::parser_operator()           // 解析运算符
{
    auto pos = cur_ptr_;
    bool find_op = false;

    if (*pos == '+') 
    {
        find_op = true;
    }
    else if (*pos == '-') 
    {
        find_op = true;
    }
    else if (*pos == '*') 
    {
        find_op = true;
    }
    else if (*pos == '/') 
    {
        find_op = true;
    }

    if (find_op) 
    {
        ++ pos;
        if (*pos == '=') 
        {
            return 2;
        }
        return 1;
    }

    return 0;
}

size_t lex_parser::parser_assign()             // 解析赋值符
{
    if (*cur_ptr_ == '=') {
        return 1;
    }
    return 0;
}

size_t lex_parser::parser_line()
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

size_t lex_parser::parser_statement()
{
    if (*cur_ptr_ == ';') {
        return 1;
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

    if (is_digital(ch)) {
        return true;
    }

    return false;
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