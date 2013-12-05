#ifndef _CRC_H
#define _CRC_H

#include <memory>
// 
// namespace klib{
// namespace encode{

//----------------------------------------------------------------------
// 需要进行封装
//int crcval=crc32(0, buff,sizeof(buff)); //使用方法 
//----------------------------------------------------------------------



/* source copy from gg's common.c */    
static unsigned int crc32_table[256];
static int crc32_initialized = 0;

static void crc32_make_table()
{     
    unsigned int h = 1;
    unsigned int i, j;

    memset(crc32_table, 0, sizeof(crc32_table));

    for (i = 128; i; i >>= 1) {
        h = (h >> 1) ^ ((h & 1) ? 0xedb88320L : 0);

        for (j = 0; j < 256; j += 2 * i)
            crc32_table[i + j] = crc32_table[j] ^ h;
    }

    crc32_initialized = 1;     
}

static unsigned int crc32(unsigned int crc, const unsigned char *buf, int len)     
{     
    if (!crc32_initialized)
        crc32_make_table();

    if (!buf || len < 0)
        return crc;

    crc ^= 0xffffffffL;

    while (len--)
        crc = (crc >> 8) ^ crc32_table[(crc ^ *buf++) & 0xff];

    return crc ^ 0xffffffffL;
}



//}}

#endif //(_CRC_H)     
