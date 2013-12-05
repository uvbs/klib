#include "bloom.h"
#include <cstdlib>

namespace klib{
namespace algo{


static HASH_FUNC HASH[] = 
{
    bloom::BKDR_hash,
    bloom::FNV_hash,
    bloom::DEK_hash 
}; 

bloom::bloom()
{
}
   
bloom::~bloom()
{
    clear();
}

bool bloom::init(size_t nsize)
{
    filter_ = new bloom_type[nsize]();
    if (NULL == filter_) {
        return false;
    }

    size_ = nsize;
    return true;
}

void bloom::set_mark(const char* v) 
{ 
    HashResult *hr = (HashResult*)calloc(1, sizeof(HashResult) + (sizeof(size_t) * HASH_RESULT) ); 

    for (int i=0; i!=HASH_RESULT; ++i) 
    {
        hr->result[i] = (HASH[i](v)) % size_; 
        // set the binary bit to 1 
        filter_[hr->result[i] / 8] |= 0x80UL >> (hr->result[i] % 8);
    }

    free(hr); 
}

bool bloom::check_mark(const char* v) 
{ 
    HashResult *hr = (HashResult*) calloc(1, sizeof(HashResult) + (sizeof(size_t) * HASH_RESULT)); 

    size_t in = HASH_RESULT; 
    for (int i=0; i != HASH_RESULT; ++i)
    {
        hr->result[i] = HASH[i](v) % size_; 
        // check this bit is "1" or not
        if (filter_[hr->result[i]/8] & (0x80UL >> (hr->result[i]%8)))
        {
            in--;
        }
    }

    free(hr); 
    return in == 0;
}

void bloom::clear()
{
    if (filter_) {
        delete[] filter_;
        filter_ = NULL;
    }

    size_ = 0;
}

/* Brian Kernighan & Dennis Ritchie hashfunction , used in Java */ 
size_t bloom::BKDR_hash(const char* str)   
{   
    register size_t hash = 0;   
    while (size_t ch = (size_t)*str++)  {          
        hash = hash * 131 + ch;  
    } 
    return hash; 
} 

/* Unix System Hashfunction , also used in Microsoft's hash_map */ 
size_t bloom::FNV_hash(const char* str)   
{   
    if(!*str) 
        return 0;   
    register size_t hash = 2166136261;   
    while (size_t ch = (size_t)*str++) {   
        hash *= 16777619;   
        hash ^= ch;   
    }   
    return hash;   
}   

/* Donald Knuth Hashfunction , presented in book <Art of Computer Programming> */ 
size_t bloom::DEK_hash(const char* str)   
{   
    if(!*str)   
        return 0;   
    register size_t hash = 1315423911;   
    while (size_t ch = (size_t)*str++)  {   
        hash = ((hash << 5) ^ (hash >> 27)) ^ ch;   
    }   
    return hash;   
}   



}}
