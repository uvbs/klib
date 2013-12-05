#pragma once

#include <string>
#include "../inttype.h"
#include "../istddef.h"

namespace klib {
namespace io {



class path
{
public:
    static bool is_file(const tstring& file)  ;
    static bool is_directory(const tstring& dir) ;
    static bool is_exists(const tstring& file) ;

public:


protected:
    tstring path_;
};

}}
