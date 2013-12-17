#ifndef _kl_lex_h_
#define _kl_lex_h_

#include <functional>

typedef std::function<bool (char)> parser_callback;

/* 词法解析器 */
class lex_parser
{
public:
    typedef char char_type;

public:
    lex_parser(const char_type* script, size_t len) : 
            script_(script), 
            len_(len), 
            cur_ptr_(script) 
    {}


public:
    size_t parser_digital();            // 解析是否是否是
    size_t parser_name();               // 解析是否是名字
    size_t parser_double();             // 解析是否是浮点数
    size_t parser_function();           // 判断是否是函数
    size_t parser_class();              // 判断是否是一个类

    size_t parser_(const char_type* pos, const parser_callback& call);
    size_t parser_(const char_type* pos, char_type ch);
    size_t parser_(const char_type* pos, char_type* str, size_t len);

    size_t skip_space();
    size_t skip(size_t len);

    std::string read(size_t len);

    // 转换函数
    double to_double(size_t len);
    int    to_int(size_t len);

public:
    static bool is_digital(char_type ch);
    static bool is_letter(char_type ch);
    static bool is_digi_letter(char_type ch);
    static bool is_space(char_type ch);

protected:
    const char_type* cur_ptr_;
    const char_type* script_;
    size_t   len_;
};



#endif // _kl_lex_h_