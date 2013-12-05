
#include "bitmap.h"
#include <string>

#ifndef NULL
#define  NULL 0
#endif

namespace klib{
namespace algo{


bitmap::bitmap()
{
    bitmap_ = NULL;

    size_ = 0;
    start_ = 0;
}

bitmap::~bitmap()
{
    clear();
}

bool bitmap::init(int size, int start)
{
    size_ = size / 8 + 1;
    start_ = start;
    bitmap_ = new char[size_];  
    if(bitmap_ == NULL)
    {
        return false;  
    }
    memset(bitmap_, 0x0, size_);
    return true;
}

bool bitmap::set(int index)
{
    int quo = (index - start_) / 8 ;  //确定所在的字节
    int remainder = (index - start_) % 8;  //字节内的偏移  
    unsigned char x = (0x1 << remainder);    
    if( quo > size_)  
        return false;

    bitmap_[quo] |= x;   //所在字节内的特定位置为1  
    return true;   
}

int bitmap::get(int i)  
{  
    int quo = (i) / 8 ;  
    int remainder = (i) % 8;  
    unsigned char x = (0x1 << remainder);  
    unsigned char res;  
    if( quo > size_)  
        return -1;  
    res = bitmap_[quo] & x;  
    return res > 0 ? 1 : 0;   
}

int bitmap::data(int index)
{
    return (index + start_);  
}

void bitmap::clear()
{
    size_ = 0;
    start_ = 0;
    delete[] bitmap_;
}


}}
