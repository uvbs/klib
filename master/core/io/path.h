#ifndef _klib_path_h
#define _klib_path_h

#include "../inttype.h"
#include "../istddef.h"

#include <string>
#include <tchar.h>
#include <Windows.h>


namespace klib {
namespace io {


/// 路径相关操作
class path
{
public:
    static bool is_file(const tstring& file)  ;
    static bool is_directory(const tstring& dir) ;
    static bool is_exists(const tstring& file) ;

    static bool has_slash(const tstring& lpszPath);
    static tstring add_slash(tstring& lpszPath );
    static tstring del_slash(tstring& lpszPath );

    static tstring extract_file_name(const tstring& szFile);
    static tstring extract_file_ext(const tstring& szFile);
    static tstring extract_path(const tstring& szFile);
    static tstring del_file_ext(const tstring& szFile);

    static tstring get_system_path();
    static tstring get_specify_path(int folderid);
    static tstring get_temp_path(const LPCTSTR  lpszPrefixString = _T("klib_"));    ///< 获取临时文件夹名字

    static BOOL get_app_path(tstring& strAppPath);      ///< 获取应用程序文件夹路径
    static BOOL get_app_file(tstring& strAppFile);      ///< 获取文件的路径
    static BOOL get_app_file_name(tstring& strFileName);   ///< 获取应用程序的名字

public:
    static BOOL create_directorys(const tstring& szPath);   ///< 递归创建目录

protected:
    tstring path_;
};

}}


#endif
