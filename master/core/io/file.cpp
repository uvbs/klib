
#include "file.h"


#pragma warning(disable: 4996)

namespace klib{
namespace io{




UINT64 file::get_file_size(tstring strFilePath)
{
    int iFileSize = 0;
    FILE *fp = _tfopen(strFilePath.c_str(), _T("rb"));
    if(NULL != fp)
    {
        fseek (fp, 0, SEEK_END );
        iFileSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        fclose(fp);
    }

    return iFileSize;
}

BOOL  file::is_file_exists(LPCTSTR pszFilePath)
{
    return GetFileAttributes(pszFilePath) != -1;
}






}}