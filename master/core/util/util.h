#pragma once

#include <string>
#include <windows.h>

#include "../istddef.h"
#include "../inttype.h"

namespace klib{
namespace core{


///< 获取应用程序文件夹路径
BOOL get_app_path(tstring& strAppPath);

///< 获取文件的路径
BOOL get_app_file(tstring& strAppFile);

///< 获取应用程序的名字
BOOL get_app_file_name(tstring& strFileName);

///< 获取文件大小
UINT64 get_file_size(tstring strFilePath);

///< 判断文件是否存在
BOOL is_file_exists(LPCTSTR pszFilePath);

// 校验和的计算
// 以16位的字为单位将缓冲区的内容相加，如果缓冲区长度为奇数，
// 则再加上一个字节。它们的和存入一个32位的双字中
USHORT check_sum(USHORT* buff, int iSize);

///< 获取cpu个数
UINT32 get_cpu_number();

///< 文件时间转换为世界时间
UINT64 FileTimeToUtc(const FILETIME* ftime);

///< 获取cpu使用率
INT32 get_cpu_usage();


///< 计算crc32值
size_t crc32(unsigned long crc, const unsigned char* buf, size_t len);


}}

