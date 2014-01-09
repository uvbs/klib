#include "path.h"



#ifdef _MSC_VER
#include <windows.h>
#include <tchar.h>
#endif

#include <vector>


namespace klib{
namespace io {




#ifdef _MSC_VER

bool path::is_file(const tstring& file)  
{
    DWORD dwAttribute = GetFileAttributes(file.c_str());
    if (INVALID_FILE_ATTRIBUTES == dwAttribute) 
    {
        return false;
    }

    return !(dwAttribute & FILE_ATTRIBUTE_DIRECTORY);
}

bool path::is_directory(const tstring& dir) 
{
    DWORD dwAttribute = GetFileAttributes(dir.c_str());
    if (INVALID_FILE_ATTRIBUTES == dwAttribute) 
    {
        return false;
    }

    BOOL bResult = (dwAttribute & FILE_ATTRIBUTE_DIRECTORY);
    return bResult != 0;
}

bool path::is_exists(const tstring& file) 
{
    DWORD dwAttribute = GetFileAttributes(file.c_str());
    if (INVALID_FILE_ATTRIBUTES == dwAttribute) 
    {
        return false;
    }

    return true;
}

bool path::has_slash(const tstring& lpszPath)
{
    if (lpszPath.empty()) 
    {
        return false;
    }

    if (lpszPath[lpszPath.size() - 1] != '\\' ||
        lpszPath[lpszPath.size() - 1] != '/') 
    {
        return true;
    }

    return false;
}

tstring path::add_slash(tstring& lpszPath )
{
    if (lpszPath.empty()) 
    {
        return lpszPath;
    }

    if (lpszPath[lpszPath.size() - 1] != '\\' &&
        lpszPath[lpszPath.size() - 1] != '/') 
    {
        lpszPath.append(_T("\\"));
    }

    return lpszPath;
}

tstring path::del_slash(tstring& lpszPath )
{
    if (lpszPath.empty()) 
    {
        return lpszPath;
    }

    if (lpszPath[lpszPath.size() - 1] != '\\' &&
        lpszPath[lpszPath.size() - 1] != '/') 
    {
        return lpszPath;
    }

    lpszPath.erase(lpszPath.end());
    return lpszPath;
}

tstring path::extract_file_name(const tstring& szFile)
{
    //@todo 需要测试
    size_t index = szFile.size() - 1;
    while (index > 0)
    {
        if (szFile[index] == _T('\\') ||
            szFile[index] == _T('/')) 
        {
            return szFile.substr(index + 1);
        }
    }

    return tstring();
}

tstring path::extract_file_ext(const tstring& szFile)
{
    size_t pos = szFile.rfind(_T("."));
    if (pos == std::string::npos) 
    {
        return tstring();
    }

    return szFile.substr(pos);
}

tstring path::extract_path(const tstring& szFile)
{
    //@todo 需要测试
    size_t index = szFile.size() - 1;
    while (index > 0)
    {
        if (szFile[index] == _T('\\') ||
            szFile[index] == _T('/')) 
        {
            return szFile.substr(0, index + 1);
        }
    }

    return szFile;
}

tstring path::del_file_ext(const tstring& szFile)
{
    size_t pos = szFile.rfind(_T("."));
    if (pos == std::string::npos) 
    {
        return szFile.substr(0, pos);
    }

    return szFile;
}

tstring path::get_system_path()
{
    TCHAR szChar[1024];
    GetWindowsDirectory(szChar, _countof(szChar));
    tstring str = szChar;
    return add_slash(str);
}

tstring path::get_temp_path(const LPCTSTR  lpszPrefixString/* = _TEXT("klib_")*/)
{
    TCHAR szChar[1024] = {0};
    TCHAR szTemp[1024] = {0};
    ::GetTempPath(_countof(szChar), szChar);
    ::GetTempFileName(szChar, lpszPrefixString, 0, szTemp);
    return szTemp;
}

BOOL path::get_app_path(tstring& strAppPath)
{
    static TCHAR pszAppPath[MAX_PATH] = {0}; 

    if (pszAppPath[0] == '\0')
    {
        const TCHAR chPathSlashes = _T('\\');
        GetModuleFileName(0, pszAppPath, MAX_PATH); 

        int iLen = _tcslen(pszAppPath);
        TCHAR* endPos = pszAppPath + iLen;
        while (*endPos != chPathSlashes) 
        {
            -- endPos;
        }

        if (*endPos != chPathSlashes) 
        {
            return FALSE;
        }

        *(endPos + 1)= _T('\0'); 
    }

    strAppPath = pszAppPath;
    return TRUE;
}

BOOL path::get_app_file(tstring& strAppFile)
{
    static TCHAR pszAppPath[MAX_PATH*2] = {0}; 

    if (pszAppPath[0] == '\0')
    {
        GetModuleFileName(0, pszAppPath, _countof(pszAppPath));
    }

    strAppFile = pszAppPath;
    return TRUE;
}

BOOL path::get_app_file_name(tstring& strFileName)
{
    static TCHAR pszAppPath[MAX_PATH] = {0}; 

    if (pszAppPath[0] == '\0')
    {
        const TCHAR chPathSlashes = _T('\\');
        GetModuleFileName(0, pszAppPath, MAX_PATH); 

        int iLen = _tcslen(pszAppPath);
        TCHAR* endPos = pszAppPath + iLen;
        while (*endPos != chPathSlashes) 
        {
            -- endPos;
        }

        if (*endPos != chPathSlashes) 
        {
            return FALSE;
        }

        strFileName = (endPos + 1);
        memmove(pszAppPath, endPos + 1, _tcslen(endPos + 1) + 1);
    }

    strFileName = pszAppPath;

    return TRUE;
}

BOOL path::create_directorys(const tstring& szPath)
{
    BOOL bSuccess = FALSE;

    tstring str_path = szPath;
    add_slash(str_path);

    std::vector<tstring> vPath;	   //存放每一层目录字符串
    tstring strTemp;			   //一个临时变量,存放目录字符串

    for (unsigned int i=0; i< str_path.size(); i++)
    {
        //如果当前字符是'\\'
        //将当前层的字符串添加到数组中
        if (str_path[i] == _T('\\') || str_path[i] == _T('/'))
        {
            vPath.push_back(strTemp);
        }
        strTemp += str_path[i];
    }// end of for

    //遍历存放目录的数组,创建每层目录
    for (std::vector<tstring>::const_iterator vIter = vPath.begin(); vIter != vPath.end(); vIter++)
    {
        //如果CreateDirectory执行成功,返回true,否则返回false
        bSuccess = CreateDirectory(vIter->c_str(), NULL) ;
    }

    return bSuccess;
}





#endif




}}
