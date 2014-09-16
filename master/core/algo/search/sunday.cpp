

#include <stdio.h>  
#include <vector>  

#include "sunday.h"

using namespace klib::algo;


sunday::sunday()
    : jump_table_inited_(false)
    , pat_start_(0)
    , pat_end_(0)
    , match_rule_(MATCH_RULE_STEP_ONE_CHAR)  
{}

int sunday::search(const char *text_src, size_t text_len)
{
    return search(text_src, text_src + text_len);
}

// 源串     [text_start, text_end)  
// 模式串   [pat_start, pat_end)  
// @return -1 没找到， else 目标相对于起始串的偏移  
int sunday::search(const char *text_start, const char *text_end)  
{     
    if (text_start >= text_end)  
    {     
        return -1;   
    }     

    if (!jump_table_inited_)  
    {     
        return -1;   
    }     

    int text_len = text_end - text_start;  
    int pat_len = pat_end_ - pat_start_;  

    for (int i=0; i<text_len-pat_len+1; )  
    {     
        bool finded = true;  
        // 从后往前匹配  
        for (int j=0; j<pat_len; ++j)  
        {     
            // 匹配不上, 跳  
            if (text_start[i+pat_len-j-1] != pat_start_[pat_len-j-1])  
            {     
                //printf("i:%d, j:%d\n", i, j);  
                //printf("text:%d [%c], pat:%d [%c] \n", i+pat_len-j-1, text_start[i+pat_len-j-1],  pat_len-j-1, pat_start_[pat_len-j-1]);  
                //printf("i:%d [%c], j:%d [%c] \n", i, text_start[i],  j, pat_start_[j]);  
                u_char next_c = (u_char)text_start[i + pat_len];  
                //printf("next c:%d, [%c], jmp:%d\n", i+pat_len, next_c, jump_table_[next_c]);  

                i += jump_table_[next_c];  

                finded = false;  
                break;  
            }     
        }     

        if (finded)  
        {     
            // 匹配上了   
            return i;  
        }  
    }  

    return -1;  
}  

// 将每一个匹配项的偏移存于pos_vec  
void sunday::search(const char *text_start, const char *text_end, std::vector<int> &pos_vec)  
{  
    int pos = 0;  

    const char *text_start_new = (const char*)text_start;  

    int pat_len = pat_end_ - pat_start_;  

    while(pos != -1)  
    {  
        pos = search(text_start_new, text_end);  

        if (pos != -1)  
        {  
            pos_vec.push_back(pos + text_start_new - text_start);  

            if (MATCH_RULE_STEP_ONE_CHAR == match_rule_)  
            {  
                text_start_new += (1 + pos);  
            }  
            else  
            {  
                text_start_new += (pat_len + pos);  
            }  

        }  
        else  
        {  
            break;  
        }  
    }  
}  

// 设置模式串  
// [pat_start, pat_end) 不含pat_end.  
void sunday::set_pattern(const char* pat_start, const char* pat_end)  
{  
    pat_start_ = pat_start;  
    pat_end_ = pat_end;  
    pre_compute(pat_start, pat_end);  
}  

// 设置匹配策略  
// 假设文本串为 "aaaaaa", 模式串为 "aaa"  
// 如果rule:MATCH_RULE_STEP_ONE_CHAR, 则会产生 4次匹配  
// 如果rule:MATCH_RULE_STEP_ONE_PATTERN, 则会产生 2次匹配  
void sunday::setmatch_rule_(int rule)  
{  
    match_rule_ = rule;  
}  

// 生成跳转表  
void sunday::pre_compute(const char* pat_start, const char* pat_end)  
{  
    if (pat_start >= pat_end)  
    {  
        return;  
    }  

    int pat_len = pat_end - pat_start ;  

    // 初始化  
    for (int i=0; i<JUMP_TABLE_LEN; ++i)  
    {  
        jump_table_[i] = pat_len + 1; // pat长度+1  
    }  

    const char* p = pat_start;  
    for (; p!=pat_end; ++p)  
    {  
        jump_table_[(u_char)(*p)] = pat_end - p;  
    }  

    jump_table_inited_ = true;  
}  

