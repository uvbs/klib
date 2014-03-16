#ifndef _klib_str_enc_h_
#define _klib_str_enc_h_

namespace klib{
namespace encrypt{


typedef unsigned char* stream_ptr;


void enc_str(unsigned char* lpszText, 
    unsigned char *lpszReturn, 
    unsigned char* lpszKey)
{
    int nTextLen = 0;
    char *cPos = NULL;
    char *pDest = NULL;
    if(lpszReturn)	// 加密
    {
        nTextLen = ::strlen(lpszText);
        pDest = new char[nTextLen + 3];	// ==\0
    }
    else	// 解密
    {
        // 查找自定的中止标记
        cPos = (stream_ptr)lpszText;
        while(true)	// 从这里可以看到，除非搜索到我们自定的中止标记，否则会一直搜索下去
        {
            if(*cPos == '=')
                if(cPos[1] == '=')
                    if(cPos[2] == '\0')
                        break;
            cPos++;
        }

        if(!cPos)	// 没有找到结束标记，也不是加密
            return;

        nTextLen = cPos - lpszText;
        pDest = (LPTSTR)lpszText;
    }

    int nKeyLen = ::strlen(lpszKey);
    int i = 0;
    int k = 0;
    int t = nTextLen;
    int cn = 0;
    for(int a = 0; a < nKeyLen; a++)	// 将密钥所有值加起来
        cn += lpszKey[a];

    for(; i < nTextLen; i++)
    {
        if(lpszReturn)	// 加密
        {
            pDest[i] = lpszText[i] + t;
            pDest[i] = pDest[i] ^ lpszKey[k];
            pDest[i] = pDest[i] ^ cn;
        }
        else	// 解密，顺序与加密时相反
        {
            pDest[i] = lpszText[i] ^ cn;
            pDest[i] = pDest[i] ^ lpszKey[k];
            pDest[i] = pDest[i] - t;
        }
        k++;
        t--;
        if(k >= nKeyLen)
            k = 0;
        if(t <= 0)
            t = nTextLen;
    }

    if(!cPos)
    {
        memcpy(pDest + nTextLen, _T("==\0"), 3 * sizeof(unsigned char));
        *lpszReturn = pDest;
    }
    else 
    {
        memset(pDest  + nTextLen, _T('\0'), sizeof(unsigned char));
    }
}



}}



#endif // _klib_str_enc_h_

