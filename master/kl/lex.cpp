
#include "lex.h"

#include <locale>

size_t lex_parser::parser_digital()
{
    return this->parser_(cur_ptr_, is_digital);
}

size_t lex_parser::parser_name()               // 解析是否是名字
{
    if (is_letter(*cur_ptr_)) 
    {
        return this->parser_(cur_ptr_ + 1, is_digi_letter);
    }
    return 0;
}

size_t lex_parser::parser_double()             // 解析是否是浮点数
{
    size_t len_main = this->parser_digital();
    const char_type* pos = cur_ptr_ + len_main;

    if (0 == this->parser_(pos, '.')) {
        return 0;
    }
    size_t len_ext = this->parser_(pos + 1, is_digital);
    return len_ext + len_main + 1;
}

size_t lex_parser::parser_function()           // 判断是否是函数
{
    return 0;
}

size_t lex_parser::parser_class()              // 判断是否是一个类
{

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

size_t lex_parser::skip_space()
{
    size_t len = this->parser_(cur_ptr_, is_space);
    cur_ptr_ += len;
    return len;
}

size_t lex_parser::skip(size_t len)
{
    cur_ptr_ += len;
    return len;
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
    if (isspace(ch)) {
        return true;
    }

    return false;
}