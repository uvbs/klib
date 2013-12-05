#pragma once

#pragma  warning(disable: 4200)

namespace klib{
namespace algo{

#define NORMAL_BLOOM_SIZE (224UL*1024UL*1024UL) // 1G 
#define HASH_RESULT     3 

typedef unsigned char BloomFilter; 
typedef size_t (*HASH_FUNC)(const char*); 

typedef struct __hash_result { 
    size_t N;   // how many result 
    size_t result[0]; 
}HashResult; 


class bloom
{
public:
    bloom();
    ~bloom();

    typedef unsigned char bloom_type;

public:
    /*初始化过滤器*/
    bool init(size_t nsize);

    /*设置标记， v是一个字符串*/
    void set_mark(const char* v) ;

    /*检测标记，检查v是否是在过滤器中，v是一个字符串*/
    bool check_mark(const char* v) ;

    /*清除内存*/
    void clear();

public:
    // hash functionS
    /* Brian Kernighan & Dennis Ritchie hashfunction , used in Java */ 
    static size_t BKDR_hash(const char* str) ;

    /* Unix System Hashfunction , also used in Microsoft's hash_map */ 
    static size_t FNV_hash(const char* str) ;

    /* Donald Knuth Hashfunction , presented in book <Art of Computer Programming> */ 
    static size_t DEK_hash(const char* str) ;

protected:
    bloom_type* filter_;
    size_t  size_;
};

}}