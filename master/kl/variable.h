
#ifndef _kl_variable_h_
#define _kl_variable_h_

#include <string>
#include <map>
#include <list>



/* 定义变量类型 */
enum var_type
{
    type_int,
    type_double,
    type_string,
    type_array,
    type_function,
    type_object,
};

/* 变量定义 */
class variable
{
public:
    var_type type_;                             // 变量类型
    union
    {
        double      double_val_;                // 实数
        int         int_val_;                   // 整型
    };
    typedef std::map<variable, variable> array_type;
    array_type      array_;                     // 数组类型
    std::string     str_val_;                   // 字符串

public:
    variable() : 
    type_(type_int), 
        int_val_(0) 
    {}

    variable(const variable& );
    variable(const char* );
    variable(double );
    variable(int);

    variable(variable&& other);
    ~variable();

    var_type    get_type() const;
    std::string to_string() const ;
    int         to_int()    const ;
    double      to_double() const ; 

    void set_type(var_type vtype) ;

    void clear();
    
    bool operator < (const variable& other)const ;
    variable& operator[] (size_t );
    variable& operator[] (const variable& );
    variable& operator[] (const char* str );

    variable& operator = (int );
    variable& operator = (const double );
    variable& operator = (const char* str);
    variable& operator = (const variable& other);
    variable& operator = (variable&& other);
    
};


#endif