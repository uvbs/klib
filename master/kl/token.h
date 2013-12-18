
#ifndef _kl_token_h_
#define _kl_token_h_

#include <string>
#include <list>

// 符号类型
enum token_type
{
    token_comment,      // comment
    token_name,         // name
    token_class,        // class 
    token_int,
//    token_keyword,      // keyword
    token_function,
    token_double,
    token_assign,
    token_line,
    token_statement,
    token_logic,        // < > || && 
    token_logic_less,
    token_logic_greater,
    token_logic_or,
    token_logic_and,
    token_operator,     // + - * / += -= *= /= 
    token_operator_add,     // +
    token_operator_minus,   // -
    token_operator_multi,       // *
    token_operator_divide,      // /
    token_operator_add_assign,      // += 
    token_operator_minus_assign,    // -= 
    token_operator_multi_assign,    // *= 
    token_operator_divide_assign,   // /= 
};

// 符号类
class token
{
public:
    void set_tks(const char* pos, size_t len) { pos_ = pos; len_ = len; }
    void set_type(token_type t) { type_ = t; }
    void set_line_no(size_t no) { line_no_ = no; }

    std::string str() const {  return std::string(pos_, len_);  }

    const char* get_pos() const { return pos_; }
    size_t      get_len() const { return len_; }
    token_type  get_type() const { return type_; }
    size_t      get_line_no() const { return line_no_; }

private:
    token_type      type_;
    const char*     pos_;
    size_t          len_;
    size_t          line_no_;
};

// 符号列表
typedef std::list<token> token_list_type;



#endif // _kl_token_h_