#include "crc23.h"

//------------------------------------------------------------------------
// 

namespace klib{
namespace encode {


crc32  g_crc32_; 

crc32::crc32()
{
    init_table();
}

void crc32::init_table()  
{  
    uint32_t c;
    int i = 0;
    int bit = 0;

    for(i = 0; i < 256; i++)
    {
        c  = (uint32_t)i;

        for(bit = 0; bit < 8; bit++)
        {
            if(c&1)
            {
                c = (c >> 1)^(0xEDB88320);
            }
            else
            {
                c =  c >> 1;
            }  
        }
        crc32_table_[i] = c;
    }
}  

uint32_t crc32::get_crc(const unsigned char *data, size_t size, uint32_t crc/* = 0*/)
{
    if (0 == data) 
        return 0;


    while(size--) 
        crc = (crc >> 8)^(crc32_table_[(crc ^ *data++) & 0xff]);  

    return crc;
}


}}