#include "variable.h"
#include <stdlib.h>
#include <string>



std::string variable::to_string() const 
{
    switch(type_)
    {
    case type_int:
        return std::to_string((long long)int_val_);

    case type_double:
        return std::to_string((long double)double_val_);

    case type_string:
        return std::string(str_val_.buff_, str_val_.len_);
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
        return double_val_;

    case type_string:
        {
            char* p;
            return strtoul(str_val_.buff_, &p, 10);
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
            return strtoul(str_val_.buff_, &p, 10);
        }
    }

    return 0;
}
