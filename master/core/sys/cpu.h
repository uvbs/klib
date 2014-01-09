
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

    // 校验和的计算
    // 以16位的字为单位将缓冲区的内容相加，如果缓冲区长度为奇数，
    // 则再加上一个字节。它们的和存入一个32位的双字中
    USHORT check_sum(USHORT* buff, int iSize);

};




}}