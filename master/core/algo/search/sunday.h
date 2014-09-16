/* 
* @desc    : Sunday String pattern matching algorithm (also support binary buf pattern match) 
* @author  : nemozhang 
* 
*/  
#ifndef _Klib_Sunday_H_  
#define _Klib_Sunday_H_  

#include <stdio.h>  
#include <vector>  

namespace klib{
namespace algo{


#ifndef u_char  
#define u_char unsigned char  
#endif  

class sunday  
{  
public:  
    enum
    {
        JUMP_TABLE_LEN                  = 256    // 跳转表大小  
    };

    enum 
    {
        MATCH_RULE_STEP_ONE_CHAR        = 0,    // 匹配上时, 跳过一个字符长度继续匹配  
        MATCH_RULE_STEP_ONE_PATTEN      = 1,    // 匹配上时, 跳过一个模式串长度继续匹配  
    };  

public:  

    sunday();

public:  
    // 源串     [text_start, text_end)  
    // 模式串   [pat_start, pat_end)  
    // @return -1 没找到， else 目标相对于起始串的偏移  
    int search(const char *text_start, const char *text_end) ;

    // 将每一个匹配项的偏移存于pos_vec  
    void search(const char *text_start, const char *text_end, std::vector<int> &pos_vec)  ;
    // 设置模式串  
    // [pat_start, pat_end) 不含pat_end.  
    void set_pattern(const char* pat_start, const char* pat_end);

    // 设置匹配策略  
    // 假设文本串为 "aaaaaa", 模式串为 "aaa"  
    // 如果rule:MATCH_RULE_STEP_ONE_CHAR, 则会产生 4次匹配  
    // 如果rule:MATCH_RULE_STEP_ONE_PATTERN, 则会产生 2次匹配  
    void setmatch_rule_(int rule) ;

private:  
    // 生成跳转表  
    void pre_compute(const char* pat_start, const char* pat_end);

private:  
    u_char          _jump_table[JUMP_TABLE_LEN];  
    bool            jump_table_inited_;  

    const char    *pat_start_;  
    const char    *pat_end_;  

    int           match_rule_;  
};


}}


#endif  