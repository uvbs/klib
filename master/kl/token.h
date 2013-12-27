
#ifndef _kl_token_h_
#define _kl_token_h_

#include <string>
#include <list>
#include <map>
#include <set>


// 符号类型
enum token_type
{
    token_none,

    token_assign,                   // =
    token_line,                     // \r\n
    token_statement,                // ??????
    token_comment,                  // comment
    token_semicolon,                // ; 分号

    token_name,                     // name, 变量名
    token_int,                      // xxx
    token_double,                   // xx.xxx
    
    token_keyword,                  // keyword
    token_class,                    // class 
    token_function,                 // function
    token_var,                      // var
    token_if,                       // if
    token_else,                     // else
    token_this,                     // $this
    token_while,                    // while
    token_do,                       // do
    token_public,                   // public
    token_private,                  // private
    token_protect,                  // protect


    token_logic,                    // < > || && 
    token_logic_less,               // <
    token_logic_greater,            // >
    token_logic_or,                 // ||
    token_logic_and,                // &&
    token_logic_not_eq,             // !=
    token_logic_eq,                 // ==
    token_logic_greater_eq,         // >=
    token_logic_less_eq,            // <=

    token_operator,                 // + - * / += -= *= /= 
    token_operator_add,             // +
    token_operator_minus,           // -
    token_operator_multi,           // *
    token_operator_divide,          // /
    token_operator_add_assign,      // += 
    token_operator_minus_assign,    // -= 
    token_operator_multi_assign,    // *= 
    token_operator_divide_assign,   // /= 

    token_bit_operation,
    token_bit_or,                   // |
    token_bit_or_assign,            // |=
    token_bit_and,                  // &
    token_bit_and_assign,           // &=
    token_bit_left_mov,             // <<
    token_bit_left_mov_assign,      // <<=
    token_bit_right_mov,            // >>
    token_bit_right_mov_assign,     // >>=
};

typedef char token_char_type;


// 关键字信息
class keyword_info
{
public:
    bool operator < (const keyword_info& other) const
    {
        if (len_ != other.len_) 
        {
            return len_ < other.len_;
        }

        return memcmp(this->name_, other.name_, this->len_) < 0;
    }
    keyword_info() {}
    keyword_info(const token_char_type* name, token_type type)
    {
        init(name, strlen(name), type);
    }

    void init(const token_char_type* name, size_t len, token_type type)
    {
        name_ = name;
        type_ = type;
        len_  = len;
    }

protected:
    const token_char_type*   name_;
    size_t                   len_;
    token_type                  type_;
};

// 符号类
class token
{
public:
    void set_tks(const token_char_type* pos, size_t len) { pos_ = pos; len_ = len; }
    void set_type(token_type t) { type_ = t; }
    void set_line_no(size_t no) { line_no_ = no; }

    std::string str() const {  return std::string(pos_, len_);  }

    const char* get_pos() const { return pos_; }
    size_t      get_len() const { return len_; }
    token_type  get_type() const { return type_; }
    size_t      get_line_no() const { return line_no_; }

    bool empty() const { return pos_ != NULL;}
private:
    token_type                  type_;
    const token_char_type*      pos_;
    size_t                      len_;
    size_t                      line_no_;
};

// 符号列表
typedef std::list<token> token_list_type;                   // 
typedef std::set<keyword_info> keyword_list_type;              // 关键字的表


#endif // _kl_token_h_