
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
    type_list,
};

/* 字符串类型 */
struct str_type
{
    char*   buff_;
    size_t  len_;
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
        str_type    str_val_;                   // 字符串
    };
    typedef std::map<variable, variable> array_type;
    array_type array_;        // 数组类型

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

    std::string to_string() const ;
    int         to_int()    const ;
    double      to_double() const ; 

    void clear();
    
    bool operator < (const variable& other)const ;
    variable& operator[] (size_t );
    variable& operator[] (const variable& );
    variable& operator[] (const char* str );

    variable& operator = (int );
    variable& operator = (const double );
    variable& operator = (const char* str);
    variable& operator = (const variable& other);
    
protected:


};

//----------------------------------------------------------------------
// 构造函数
variable::variable(const variable& var)
{
    if (this == &var) {
        return;
    }

    *this = var;
}

variable::variable(const char* str)
{
    *this = str;
}

variable::variable(double var)
{
    *this = var;
}

variable::variable(int var)
{
    *this = var;
}

//----------------------------------------------------------------------
//
variable::variable(variable&& other)
{
    clear();

    if (other.type_ == type_string) 
    {
        this->type_ = type_string;
        this->str_val_.buff_ = other.str_val_.buff_;
        this->str_val_.len_  = other.str_val_.len_;

        other.str_val_.buff_ = 0;
        other.str_val_.len_  = 0;
        other.type_ = type_int;
    }
    else
    {
        *this = other;
    }
}

//----------------------------------------------------------------------
//

variable::~variable()
{
    clear();
}

//----------------------------------------------------------------------
//
void variable::clear()
{
    if (type_ == type_string) 
    {
        if (str_val_.buff_) 
        {
            delete str_val_.buff_;
            str_val_.buff_ = 0;
            str_val_.len_ = 0;
        }
        
        type_ = type_int;
    }
}

bool variable::operator < (const variable& other)const 
{
    if (this->type_ == other.type_) 
    {
        if (this->type_ == type_int) 
        {
            return this->int_val_ < other.int_val_;
        }
        else if (this->type_ == type_double) 
        {
            return this->double_val_ < other.double_val_;
        }
        else if (this->type_ == type_string) 
        {
            if (this->str_val_.len_ == other.str_val_.len_) 
            {
                return strcmp(this->str_val_.buff_, other.str_val_.buff_) < 0;
            }
            else
            {
                return this->str_val_.len_ < other.str_val_.len_;
            }
        }
        else if (this->type_ == type_array) 
        {
            if (array_.size() == other.array_.size()) 
            {
                auto itr_other = other.array_.begin();
                for (auto itr = array_.begin(); itr != array_.end(); ++itr)
                {
                    if (itr->first < itr_other->first) 
                    {
                        return true;
                    }
                }

                return false;
            }
            else
            {
                return this->array_.size() < other.array_.size();
            }
        }
    }
    else 
    {
        return this->type_ < other.type_;

    }
    return true;
}

variable& variable::operator[] (size_t )
{
    clear();

    return *this;
}

variable& variable::operator[] (const variable& var)
{
    clear();

    this->type_ = type_array;

    return array_[var];
}

variable& variable::operator[] (const char* str )
{
    clear();

    this->type_ = type_array;

    variable v = str;
    return array_[v];
}

variable& variable::operator = (int var)
{
    clear();

    this->type_ = type_int;
    int_val_ = var;
    return *this;
}

variable& variable::operator = (const double var)
{
    clear();

    this->type_ = type_double;
    double_val_ = var;
    return *this;
}

variable& variable::operator = (const char* str)
{
    if (NULL == str) 
    {
        return *this;
    }

    clear();

    this->type_ = type_string;

    size_t len = strlen(str) ;
    this->str_val_.buff_ = new char[len + 1];
    if (NULL == this->str_val_.buff_) {
        return *this;
    }

    memcpy(this->str_val_.buff_, str, len);
    this->str_val_.buff_[len] = '\0';
    this->str_val_.len_ = len;
    return *this;
}

variable& variable::operator = (const variable& other)
{
    if (this == &other) 
    {
        return *this;
    }

    clear();

    if (type_string == other.type_) 
    {
        *this = other.str_val_.buff_;
    }
    else
    {
        this->type_ = other.type_;
        this->int_val_ = other.int_val_;
        this->double_val_ = other.double_val_;
        this->array_ = other.array_;
    }

    return *this;
}