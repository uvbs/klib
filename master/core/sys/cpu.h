
#include "../istddef.h"
#include "../inttype.h"


namespace klib {
namespace sys {


// cpu相关操作的封装
class cpu
{
public:

    ///< 获取cpu个数
    UINT32 get_cpu_number();

    ///< 获取cpu使用率
    INT32 get_cpu_usage();

};




}}