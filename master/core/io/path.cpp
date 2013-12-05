#include "path.h"

#ifdef _MSC_VER
#include <windows.h>
#endif

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


#endif


}}
