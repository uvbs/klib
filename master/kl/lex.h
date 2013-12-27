#ifndef _kl_lex_h_
#define _kl_lex_h_

#include <vector>
#include <functional>

#include "token.h"

typedef char char_type;

/* parser result */
class parser_result
{
public:
    parser_result() : len_(0), type_(token_none) {}
    parser_result(size_t len) : len_(len), type_(token_none)  {}
    parser_result(size_t len, token_type type) : len_(len), type_(type) {}

    size_t length() const { return len_; }
    token_type type() const { return type_; }

protected:
    size_t          len_;       // parsed length 
    token_type      type_;      // token type
};

/* parser error info */
struct parsser_error_info
{
    const char_type* err_msg_;
    const char_type* err_pos_;
    size_t           line_no_;
};

// call back function type
typedef std::function<bool (char)> parser_callback;

// parser worker function define
typedef std::function<parser_result()> parser_worker;

// parser function information
struct parser_func_info
{
    parser_func_info() {}
    parser_func_info(token_type type, parser_worker f) : 
            type_(type), func_(f) {}

    token_type      type_;
    parser_worker   func_;
};
typedef std::vector<parser_func_info> parser_func_list;


/* lex parser */
class lex_parser
{
public:

public:
    lex_parser(const char_type* script, size_t len) : 
            script_buff_(script), 
            script_len_(len), 
            cur_ptr_(script), 
            line_no_(1)
    {
        end_ptr_ = script_buff_ + len;
        init_keyword_list(); 
    }


public:
    bool parser_script(token_list_type& thelist, parsser_error_info& err_info);           // parser script strings
    bool is_finished() const ;

    const char_type* get_pos() const {  return cur_ptr_;}  
    size_t get_line_no() const { return line_no_; }

protected:
    bool get_parser_funcs(parser_func_list& thelist);       // 获取解析函数列表
    bool init_keyword_list();

    parser_result parser_comment();            // 注释
    parser_result parser_int();                // 是否是否是
    parser_result parser_name();               // 是否是名字
    parser_result parser_string();             // 是否是字符串
    parser_result parser_keyword();            // 判断是否是函数
    parser_result parser_double();             // 是否是浮点数
    parser_result parser_logic();              // 逻辑运算符
    parser_result parser_operator();           // 运算符
    parser_result parser_bracket();            // 括号 () {}[]
    parser_result parser_assign();             // 赋值符
    parser_result parser_line();               // 一行结束
    parser_result parser_statement();          // 语句结束

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
    static bool is_underline(char_type ch);
    static bool is_digi_letter_underline(char_type ch);
    bool is_space(char_type ch);

protected:
    const char_type* cur_ptr_;
    const char_type* end_ptr_;
    const char_type* script_buff_;
    size_t   script_len_;
    size_t   line_no_;
    keyword_list_type keyword_list_;
};



#endif // _kl_lex_h_