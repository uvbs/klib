#pragma once

#include <windows.h>
#include <math.h>
#include <memory>

namespace klib
{
namespace algo
{

//------------------------------------------------------------------------
// ld×Ö·û´®Æ¥ÅäËã·¨
//------------------------------------------------------------------------
class ld
{
public:
    ld() : m_pszStrA(NULL),m_pszStrB(NULL),
           m_iStrAlen(0), m_iStrBlen(0)  {}

    //------------------------------------------------------------------------
    // Remarks:
    //      ÊäÈëÁ½¸ö×Ö·û´®
    //------------------------------------------------------------------------
    void init(const char* pszStrA, const char* pszStrB)
    {
        init(pszStrA, strlen(pszStrA), pszStrB, strlen(pszStrB));
    }

    void init(const char* pszStrA, int iLenStrA,
                   const char* pszStrB, int iLenStrB)
    {
        m_pszStrA = pszStrA;
        m_pszStrB = pszStrB;

        m_iStrAlen = iLenStrA;
        m_iStrBlen = iLenStrB;
    }

    //------------------------------------------------------------------------
    // Remarks:
    //      ¼ÆËã²¢·µ»ØÁ½¸ö×Ö·û´®µÄ±à¼­¾àÀë
    //------------------------------------------------------------------------
    UINT distance()
    {
        if (m_iStrAlen == 0) { return m_iStrBlen;   }
        if (m_iStrBlen == 0) { return m_iStrAlen;   }

        int j = min(m_iStrAlen, m_iStrBlen) ;

        int tP1, tP2;
        
        tP1 = -1;
        tP2 = -1;

        for (auto i=0; i<j; ++i)
        {
            if (m_pszStrA[i] != m_pszStrB[i]) {
                tP1 = i;
                break;
            }
        }

        if (tP1 == -1) {
            return ::abs(m_iStrAlen - m_iStrBlen);
        }
        
        for (auto i=0; i < j - tP1; ++i)
        {
            if (m_pszStrA[m_iStrAlen - i - 1] != m_pszStrB[m_iStrBlen - i - 1]) {
                tP2 = i;
                break;
            }
        }

        if (tP2 == -1) {
            return ::abs(m_iStrAlen - m_iStrBlen);
        }

        int tmpLen = m_iStrAlen - tP1 - tP2;
        int* tA = new int[tmpLen + 1];
        if (NULL == tA) {
            return -1;
        }
        std::shared_ptr<int> pt(tA, [](int* p){ delete[] p; });
        for (int i=0; i<=tmpLen; ++i)
        {
            tA[i] = i;
        }
           
        int tN1, tN2, tN3;
        for (int i=0; i<m_iStrBlen - tP1 - tP2 - 1; ++i)
        {
            tN1 = tA[0];
            tN2 = tN1 + 1;
            for (int j=1; j<=tmpLen; ++ j)
            {
                if (m_pszStrA[m_iStrAlen - tP2 - j] == m_pszStrB[m_iStrBlen - tP2 - i - 1]) {
                    tN3 = tN1;
                }
                else {
                    tN3 = min(tA[j], tN1);
                    tN3 = min(tN3, tN2) + 1;
                }

                tA[j - 1] = tN2;
                tN2 = tN3;
                tN1 = tA[j];
            }

            tA[tmpLen] = tN2;
        }

        return tA[tmpLen];
    }
    
protected:
    const char* m_pszStrA;
    const char* m_pszStrB;

    int         m_iStrAlen;
    int         m_iStrBlen;
};


}
}