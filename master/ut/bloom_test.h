#ifndef _klib_bloom_test_h
#define _klib_bloom_test_h

#include <algo/bloom.h>

using namespace klib::algo;

TEST(bloom, 1)
{
    bloom bloom_;

    bloom_.init(NORMAL_BLOOM_SIZE);

    for(int i=0; i!=10000; i++) 
    {
        char buf[16] = {0};
        sprintf(buf, "%d", i); 
        bloom_.set_mark(buf);
    }
    printf("BloomFilter Build Ok\n"); 

    for(int i=9995;i!=10010;i++) { 
        char buf[16] = {0}; 
        sprintf(buf, "%d", i); 
        if (bloom_.check_mark(buf))
            printf("[FOUND] %d\n",i); 
    }

    Sleep(1000);
}

#endif
