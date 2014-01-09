
#include "../istddef.h"
#include "../inttype.h"

#include <Windows.h>
#include <tchar.h>

namespace klib{
namespace io{


class file
{
public:
    static UINT64 get_file_size(tstring strFilePath);      ///< 获取文件大小
    static bool is_file_exists(LPCTSTR pszFilePath);       ///< 判断文件是否存在

    static bool write_file_content(const tstring& src_file, const LPCTSTR lpszContent, size_t nlen);

};



}}