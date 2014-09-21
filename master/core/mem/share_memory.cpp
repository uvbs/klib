#include "share_memory.h"
#include <Windows.h>
#include <tchar.h>

namespace klib{
namespace mem{


share_memory::share_memory(void) : share_mem_handle_(NULL)
{
}

share_memory::~share_memory(void)
{
    close();
}

bool share_memory::create(tstring name, size_t nSize )
{
    share_mem_handle_ = CreateFileMapping(INVALID_HANDLE_VALUE,NULL, PAGE_READWRITE, 0, nSize, name.c_str());
    if(share_mem_handle_)
    {
        share_mem_addr_ = MapViewOfFile(share_mem_handle_, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if(share_mem_addr_ == NULL)
        {
            MessageBox(NULL, _T("映射文件映射到调用进程地址时出错"), _T("错误"), MB_OK);
            return false;
        }
        return true;
    }
    else
    {
        MessageBox(NULL, _T("创建文件映射失败"), _T("错误"), MB_OK);
        return false;
    }
    share_mem_size_ = nSize;

    return true;
}

bool share_memory::open(tstring name)
{
    HANDLE  handle = OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE,
        FALSE,
        name.c_str());
    if (NULL == handle) {
        return false;
    }
    share_mem_handle_ = handle;
    share_mem_size_   = GetFileSize(handle, NULL);

    share_mem_addr_ = MapViewOfFile(handle, // Handle to mapping object. 
        FILE_MAP_ALL_ACCESS,               // Read/write permission. 
        0,                                 // Max. object size. 
        0,                                 // Size of hFile. 
        0);                                // Map entire file. 

    if (share_mem_addr_ == NULL) 
    { 
        return false;
    } 

    return true;
}

void share_memory::close()
{
    CloseHandle(share_mem_handle_);
}

std::string share_memory::read_string()
{
    std::string str = (char*)share_mem_addr_;
    return std::move(str);
}

void share_memory::write_string(string& content)
{
    size_t nCopySize = content.size() >= share_mem_size_ ? share_mem_size_ - 1 : content.size();

    memcpy(share_mem_addr_, (void*)(content.c_str()), nCopySize);
    ((char*)share_mem_addr_)[nCopySize] = '\0';
}

bool share_memory::read(char* pszBuff, size_t nReadLen, size_t nStartPos, size_t* pReadedLen)
{
    if (NULL == pszBuff) 
    {
        return false;
    }

    // 确定有多少数据可以被读取
    size_t nCopySize = 0;
    if (share_mem_size_ > (nStartPos + nReadLen)) 
    {
        nCopySize = nReadLen;
    }
    else
    {
        nReadLen = share_mem_size_ - nStartPos;
    }

    // 读取数据
    memcpy(pszBuff, (const char*)share_mem_addr_ + nStartPos, nCopySize);
    if (pReadedLen) {
        *pReadedLen = nCopySize;
    }

    return true;
}

bool share_memory::write(size_t nWritePos, const char* pSrc, size_t nDataLen, size_t* pWritedLem)
{
    // 计算能够写的长度
    size_t nWritedLen;
    if (share_mem_size_ > (nWritePos + nDataLen)) 
    {
        nWritedLen = nDataLen;
    }
    else 
    {
        nWritedLen = share_mem_size_ - nWritePos;
    }

    // 写入共享内存
    memcpy((char*)share_mem_addr_ + nWritePos, pSrc, nWritedLen);

    if (pWritedLem) 
    {
        * pWritedLem = nWritedLen;
    }

    return true;
}


}}
