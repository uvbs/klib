#ifndef _kl_lex_h_
#define _kl_lex_h_

#include <vector>
#include <functional>

#include "token.h"

// 解析回调函数类型
typedef std::function<bool (char)> parser_callback;

// 解析函数信息
struct parser_func_info
{
    token_type      type_;
    size_t          len;
    std::function<size_t()> func;
};
typedef std::vector<parser_func_info> parser_func_list;

/* 词法解析器 */
class lex_parser
{
public:
    typedef char char_type;

public:
    lex_parser(const char_type* script, size_t len) : 
            script_(script), 
            len_(len), 
            cur_ptr_(script), 
            line_no_(1)
    {}


public:
    bool init_keyword_list();
    bool get_parser_funcs(parser_func_list& thelist);       // 获取解析函数列表

    const char_type* get_pos() const {  return cur_ptr_;}  
    size_t get_line_no() const { return line_no_; }

    size_t parser_comment();            // 解析注释
    size_t parser_int();                // 解析是否是否是
    size_t parser_name();               // 解析是否是名字
    size_t parser_keyword();            // 判断是否是函数
    size_t parser_double();             // 解析是否是浮点数
    size_t parser_class();              // 判断是否是一个类
    size_t parser_logic();              // 解析逻辑运算符
    size_t parser_operator();           // 解析运算符
    size_t parser_assign();             // 解析赋值符
    size_t parser_line();               // 解析一行结束
    size_t parser_statement();          // 解析语句结束

    size_t parser_(const char_type* pos, const parser_callback& call);
    size_t parser_(const char_type* pos, char_type ch);
    size_t parser_(const char_type* pos, char_type* str, size_t len);
    size_t parser_until(const char_type* pos, char_type ch);
    size_t parser_until(const char_type* pos, char_type* str, size_t len);
    size_t parser_until(const char_type* pos, const parser_callback& call);

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
    bool is_space(char_type ch);

protected:
    const char_type* cur_ptr_;
    const char_type* script_;
    size_t   len_;
    size_t   line_no_;
    keyword_list_type keyword_list_;
};



#endif // _kl_lex_h_