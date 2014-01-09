
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

bool file::is_file_exists(LPCTSTR pszFilePath)
{
    return GetFileAttributes(pszFilePath) != -1;
}

bool file::write_file_content(const tstring& src_file, const LPCTSTR lpszContent, size_t nlen)
{
    FILE* fFile;
    _tfopen_s(&fFile, src_file.c_str(), _T("wb"));

    size_t szTextLen = nlen;
    size_t lWrite    = fwrite(lpszContent, sizeof(TCHAR), szTextLen,  fFile);
    fclose(fFile);

    if (lWrite != lWrite)
        return false ;
    return true;
}




}}