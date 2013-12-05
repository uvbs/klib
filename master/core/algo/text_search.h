#pragma once

namespace klib{
namespace algo{


#define  MIN_VISIABLE_TEXT_LEN          (5)
class text_search
{
public:
    template<typename CharT>
    struct stFindItem {
        CharT* pSzPos;
        UINT   uLen;
    };

    /*
    * @brief 搜索可见字符，并在items中返回可见的字符串
    * pBuff       :       字节流缓冲区
    * uBuffLen    :       缓冲区的长度
    * CharT       :       表示要查找可见字符的类型（char、wchar_t两种类型）
    */
    template<typename CharT, UINT uMinVisiableTextLen>
    bool search(void* pBuff, UINT uBuffLen, std::vector<stFindItem<CharT>>& items)
    {
        UINT uVisiableCount = 0;
        stFindItem<CharT> item;
        CharT* pszFindPos = NULL;

        CharT* pszEndPos = (CharT*) ((char*)pBuff + uBuffLen);
        CharT* pos = (CharT*) pBuff;
        while (pos + 1 <= pszEndPos) 
        {
            if (IsPrint<CharT>(*pos)) 
            {
                if (uVisiableCount == 0) 
                {
                    pszFindPos = pos;
                }
                ++ uVisiableCount;
            }
            else 
            {
                if (uVisiableCount >= uMinVisiableTextLen) 
                {
                    item.pSzPos = pszFindPos;
                    item.uLen   = uVisiableCount;
                
                    items.push_back(item);
                }
                uVisiableCount = 0;
            }
            
            ++ pos;
        }

        if (uVisiableCount >= uMinVisiableTextLen) 
        {
            item.pSzPos = pszFindPos;
            item.uLen   = uVisiableCount;
                
            items.push_back(item);
        }

        return !items.empty();
    }

    /*
    * @brief 判断是否是可见字符
    */
    template<typename CharT>
    BOOL IsPrint(CharT ch) 
    {
        if (sizeof(CharT) == sizeof(wchar_t)) 
        {
            return my_iswprint(ch);
        }
        
        return _ismbcprint(ch);
    }

    /*
    * @brief 判断是否是可见字符，unicode版本
    *      是可见字符返回TRUE，非可见字符返回FALSE
    */
    static BOOL my_iswprint(wchar_t ch) 
    {
        return ch >= 0x20 && ch <= 0x7E;
    }

    /*
    * @brief 搜索出现二进制数据值出现的位置(成功返回TRUE，失败返回FALSE)
    * index : 为出现二进制的索位置
    */
    template<typename CharT>
    BOOL search(const char* pszBuff, int iLen, int& index)
    {
        CharT ch;
        for (int ipos=0; ipos<iLen; ++ipos) 
        {
            ch = (CharT) pszBuff[ipos];

            // 是ASCII码
            if (IsPrint<CharT>(ch) || ch == 0x0A || ch == 0x0D) 
            {
                continue;
            }
            else 
            {
                index = ipos;
                return TRUE;
            }
        }

        return FALSE;
    }

    /*
    * @brief 判断是否是标点符号
    */
    template<typename CharT>
    static BOOL IsPunctuation(CharT ch)
    {
        if (ch >= 33 && ch <= 47) 
        {
            return TRUE;
        }

        if (ch >= 58 && ch <= 64) 
        {
            return TRUE;
        }

        if (ch >= 91 && ch <= 96) 
        {
            return TRUE;
        }

        if (ch >= 123 && ch <= 126) 
        {
            return TRUE;
        }

        return FALSE;
    }
};



}}

