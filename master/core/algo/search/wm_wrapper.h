#ifndef _klib_wm_wrapper_h
#define _klib_wm_wrapper_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include "../common/algo_def.h"

namespace klib{
namespace algo{


class wm_wrapper
{
public:
    wm_wrapper(void);
    ~wm_wrapper(void);
    
    #define HASHTABLESIZE  (256*256)
    #define HASH_TYPE      short
    #define SHIFTTABLESIZE (256*256)

    // 添加模式
    bool add_pattern(const char* pszPatten, int iLen, void* user_data = nullptr);
    bool add_pattern(const char* pszPatten, void* user_data = nullptr);

    // 添加完成后构造上下文
    bool compile();
    size_t size();

    void set_callback(search_match_callback call, void* pcall_this);
   
    // 搜索目标
    bool search(const char* pszSearch, int nLen);
    
protected:

    typedef struct wm_pattern_struct
    {
        struct wm_pattern_struct *next;
        unsigned char *psPat;   //pattern array
        unsigned int  psLen;    //length of pattern in bytes
        void*         user_data;
    } WM_PATTERN_STRUCT;

    typedef struct wm_struct
    {
        WM_PATTERN_STRUCT *plist; //pattern list
        WM_PATTERN_STRUCT *msPatArray; //array of patterns
        unsigned short *msNumArray; //array of group counts, # of patterns in each hash group
        int msNumPatterns; //number of patterns loaded
        unsigned msNumHashEntries;
        HASH_TYPE *msHash; //last 2 characters pattern hash table
        unsigned char* msShift; //bad word shift table
        HASH_TYPE *msPrefix; //first 2 characters prefix table
        int msSmallest; //shortest length of all patterns
    } WM_STRUCT;

    WM_STRUCT * wmNew();
    void wmFree(WM_STRUCT *ps);
    int wmAddPattern(WM_STRUCT *ps, unsigned char *P, int m, void* user_data = nullptr);
    unsigned HASH16(unsigned char *T);
    int wmPrepPatterns(WM_STRUCT *ps);
    bool wmSearch(WM_STRUCT *ps, void* search_data, unsigned char *Tx, int n);

    void sort(wm_wrapper::WM_STRUCT *ps);
    void wmPrepHashedPatternGroups(WM_STRUCT *ps); //计算有多少个不同哈希值，且从小到大
    void wmPrepShiftTable(WM_STRUCT *ps);
    void wmPrepPrefixTable(WM_STRUCT *ps);
    void wmGroupMatch(void* search_data,
        WM_STRUCT *ps, //后缀哈希值相同，比较前缀以及整个字符匹配
        int lindex, 
        unsigned char *Tx, 
        unsigned char *T);

protected:
    static int Match(
        void* pthis,
        void * user_data, 
        matched_patten* pattern,
        int  index
    );

protected:
    WM_STRUCT * wm_handle_;
    size_t      pattern_num_;
    
    void*                  callback_this_;
    search_match_callback  callback_func_;
};


}}



#endif
