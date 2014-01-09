
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
    static BOOL is_file_exists(LPCTSTR pszFilePath);       ///< 判断文件是否存在



};



}}