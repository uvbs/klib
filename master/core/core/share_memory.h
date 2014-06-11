#ifndef _klib_share_memory_h
#define _klib_share_memory_h
#include <string>
#include <wtypes.h>
#include <winbase.h>
using namespace std;

#include "../istddef.h"

namespace klib{
namespace mem{


using namespace klib;


/*
 内存共享类
*/
class share_memory
{
public:
	share_memory(void);
	~share_memory(void);

public:
    /**
     * @brief 创建共享内存
     * @param  name  共享内存的名字
     * @param  nSize 共享内存的大小
     */
	void create(tstring name, size_t nSize);

    /**
     * 关闭共享内存
     */
    void close();

    /**
     * @brief 获取共享内存的长度
     */
    size_t size() { return m_buffer_size; }

    /**
     * @brief 获取共享内存地址
     */
    void*  get_addr() { return m_pShareMemoryAddress; }

    /**
     * @brief 读取内存，以string返回
     */
	string read_string();

    /**
     * @brief 将string写入共享内存
     */
	void write_string(string& content);

    /**
     * @brief 从缓冲区中读取一定长度的数据
     * @param  pszBuff    读取出来的存放的缓冲区
     * @param  nReadLen   要读的长度
     * @param  nStartPos  开始读取的位置
     * @param  pReadedLen 返回实际读取的长度
     */
    bool read(char* pszBuff, size_t nReadLen, size_t nStartPos, size_t* pReadedLen);

    /**
     * @brief 将缓冲区的数据写入共享内存
     * @param  nWritePos   要写入共享内存数据的起始位置
     * @param  pSrc        要写入共享内存的数据地址
     * @param  nDataLen    要写入共享内存的长度
     * @param  pWritedLem  实际被写入的长度
     */
    bool write(size_t nWritePos, const char* pSrc, size_t nDataLen, size_t* pWritedLem);

private:
	HANDLE m_hSharememoryHandle;            ///< 共享内存句柄
	void  *m_pShareMemoryAddress;           ///< 共享内存获取的地址

	size_t m_buffer_size;                   ///< 共享内存大小
};



}}

#endif
