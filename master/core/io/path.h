#pragma once

#include "../inttype.h"
#include "../istddef.h"

#include <Windows.h>
#include <string>


namespace klib {
namespace io {



class path
{
public:
    static bool is_file(const tstring& file)  ;
    static bool is_directory(const tstring& dir) ;
    static bool is_exists(const tstring& file) ;

public:
    ///< 获取应用程序文件夹路径
    BOOL get_app_path(tstring& strAppPath);

    ///< 获取文件的路径
    BOOL get_app_file(tstring& strAppFile);

    ///< 获取应用程序的名字
    BOOL get_app_file_name(tstring& strFileName);


protected:
    tstring path_;
};

}}
