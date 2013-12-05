#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

namespace klib{
namespace algo{


class wm_wrapper
{
public:
    wm_wrapper(void);
    ~wm_wrapper(void);
    
    #define HASHTABLESIZE (256*256)
    #define MAXLEN 256
    #define HASH_TYPE short
    #define SHIFTTABLESIZE (256*256)

    // 添加模式
    void AddPattern(const char* pszPatten, int iLen);
    void AddPattern(const char* pszPatten);

    // 添加完成后构造上下文
    void BuildContext();

    // 搜索目标
    void Search(const char* pszSearch, int nLen);

    // 获取结果（一次搜索只能被获取一次）
    bool GetResult(std::vector<std::string>& vecResult);

protected:

    typedef struct wm_pattern_struct
    {
        struct wm_pattern_struct *next;
        unsigned char *psPat; //pattern array
        unsigned psLen; //length of pattern in bytes
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
    int wmAddPattern(WM_STRUCT *ps, unsigned char *P, int m);
    unsigned HASH16(unsigned char *T);
    int wmPrepPatterns(WM_STRUCT *ps);
    void wmSearch(WM_STRUCT *ps, unsigned char *Tx, int n);

    void sort(wm_wrapper::WM_STRUCT *ps);
    void wmPrepHashedPatternGroups(WM_STRUCT *ps); //计算有多少个不同哈希值，且从小到大
    void wmPrepShiftTable(WM_STRUCT *ps);
    void wmPrepPrefixTable(WM_STRUCT *ps);
    void wmGroupMatch(WM_STRUCT *ps,//后缀哈希值相同，比较前缀以及整个字符匹配
        int lindex, unsigned char *Tx, unsigned char *T);

    int m_nline ;
    int m_nfound ;
    std::vector<std::string> m_vecResult;

    WM_STRUCT * m_pWmStruct;
    
};


}}

