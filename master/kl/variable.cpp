#include "variable.h"
#include <stdlib.h>
#include <string>

var_type variable::get_type() const
{
    return type_;
}

std::string variable::to_string() const 
{
    switch(type_)
    {
    case type_int:
        return std::to_string((long long)int_val_);

    case type_double:
        return std::to_string((long double)double_val_);

    case type_string:
        return this->str_val_;
    }

    _ASSERT(0);
    return std::string();
}

int variable::to_int()    const 
{
    switch(type_)
    {
    case type_int:
        return int_val_;

    case type_double:
        return (int) double_val_;

    case type_string:
        {
            char* p;
            return strtoul(str_val_.c_str(), &p, 10);
        }        
    }

    return 0;
}

double variable::to_double() const 
{
    switch(type_)
    {
    case type_int:
        return int_val_;

    case type_double:
        return double_val_;

    case type_string:
        {
            char* p;
            return strtoul(str_val_.c_str(), &p, 10);
        }
    }

    return 0;
}

//----------------------------------------------------------------------
void variable::set_type(var_type vtype) 
{
    this->type_ = vtype;
}

//----------------------------------------------------------------------
// ¹¹Ôìº¯Êý
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
        this->str_val_ = std::move(other.str_val_);
    }
    else if (other.type_ == type_int) 
    {
        this->int_val_ = other.int_val_;
    }
    else if (other.type_ == type_double)
    {
        this->double_val_ = other.double_val_;
    }
    else if (other.type_ == type_array)
    {
        this->array_ = std::move(other.array_);
    }
    else if (other.type_ == type_function)
    {
        this->str_val_ = std::move(other.str_val_);
    }

    this->type_  = other.type_;
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
        this->str_val_.swap(std::string());

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
            if (this->str_val_.size() == other.str_val_.size()) 
            {
                return strcmp(this->str_val_.c_str(), other.str_val_.c_str()) < 0;
            }
            else
            {
                return this->str_val_.size() < other.str_val_.size();
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
    return array_[std::move(v)];
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
        return *this;

    clear();

    this->type_ = type_string;

    size_t len = strlen(str) ;
    this->str_val_ = std::string(str, len);
    return *this;
}

variable& variable::operator = (const variable& other)
{
    if (this == &other) 
        return *this;

    clear();

    if (type_string == other.type_) 
    {
        this->str_val_ = other.str_val_;
    }
    else
    {
        this->int_val_ = other.int_val_;
        this->double_val_ = other.double_val_;
        this->array_ = other.array_;
    }

    this->type_ = other.type_;

    return *this;
}

variable& variable::operator = (variable&& other)
{
    if (this == &other) 
        return *this;

    clear();

    this->int_val_ = other.int_val_;
    this->double_val_ = other.double_val_;
    this->array_ = std::move(other.array_);

    this->str_val_ = std::move(other.str_val_);
    this->type_ = other.type_;

    return *this;
}
