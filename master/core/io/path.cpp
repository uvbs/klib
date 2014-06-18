#include "path.h"
#include "../core/klib_core.h"


#ifdef _MSC_VER
#include <windows.h>
#include <tchar.h>
#include <Shlobj.h>
#endif

#include <vector>

#pragma warning(disable: 4996)


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
    if (szFile.empty()) 
    {
        return tstring();
    }

    //@todo 需要测试
    int64_t index = szFile.size() - 1;
    while (index >= 0)
    {
        if (szFile[index] == _T('\\') ||
            szFile[index] == _T('/')) 
        {
            return szFile.substr(index + 1);
        }

        -- index;
    }

    return szFile;
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

tstring path::get_specify_path(int folderid)
{ 
    TCHAR szbuf[MAX_PATH * 2] = {0};
    HRESULT ret = SHGetFolderPath(NULL, folderid, NULL, 0, szbuf);
    KLIB_ASSERT(SUCCEEDED(ret));

    return tstring(szbuf);
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

BOOL path::get_app_patha(std::string& strAppPath)
{
    static char pszAppPath[MAX_PATH] = {0}; 

    if (pszAppPath[0] == '\0')
    {
        const char chPathSlashes = '\\';
        GetModuleFileNameA(0, pszAppPath, MAX_PATH); 

        int iLen = strlen(pszAppPath);
        char* endPos = pszAppPath + iLen;
        while (*endPos != chPathSlashes) 
        {
            -- endPos;
        }

        if (*endPos != chPathSlashes) 
        {
            return FALSE;
        }

        *(endPos + 1)= '\0';
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
    TCHAR szFullfilepath[4*MAX_PATH] = {0};
    _tcsncpy(szFullfilepath, szPath.c_str(), _countof(szFullfilepath));
    int len = _tcslen(szFullfilepath);
    TCHAR* pos = (TCHAR*) &szFullfilepath[0];
    pos += len;
    while( (*pos != '\\') && (*pos != '/')){ pos--; }
    *pos = '\0';

    TCHAR filedirectory[MAX_PATH] = {0};
    TCHAR* token;
    TCHAR seps[] = _T("\\/");

    token = _tcstok(szFullfilepath, seps);
    if (token) {
        _tcsncpy(filedirectory, token, MAX_PATH);
        token = _tcstok( NULL, seps );
    }
    while( token != NULL )
    {
        _tcscat(filedirectory, _T("\\"));
        _tcscat(filedirectory, token);
        if (FALSE == CreateDirectory(filedirectory, NULL)) {
            if (ERROR_ALREADY_EXISTS != GetLastError()) {
                return FALSE;
            }
        }
        token = _tcstok( NULL, seps );
    }

    return TRUE;
}

BOOL path::delete_directorys(const tstring& szPath)
{
    WIN32_FIND_DATA   finddata;
    HANDLE   hfind;

    tstring str_search = szPath;
    add_slash(str_search);

    str_search.append(_T("*.*"));

    hfind = FindFirstFile(str_search.c_str(), &finddata);
    if(INVALID_HANDLE_VALUE == hfind)
        return FALSE;

    do
    {
        if(_tcscmp(finddata.cFileName, _T("."))==0 ||
           _tcscmp(finddata.cFileName, _T(".."))==0)
        {
            continue;
        }

        tstring s_dir = str_search;
        add_slash(s_dir);
        s_dir.append(finddata.cFileName);

        if(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            delete_directorys(s_dir.c_str());
        else
            DeleteFile(s_dir.c_str());

    } while(FindNextFile(hfind, &finddata));

    FindClose(hfind);
    if(RemoveDirectory(szPath.c_str()))
    {
        return   TRUE;
    }
    else
    {
        return   FALSE;
    }
}



#endif




}}
