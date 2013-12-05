//#include "StdAfx.h"
#include "wm_wrapper.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

using namespace klib::algo;


wm_wrapper::wm_wrapper(void)
{
    m_nline = 1;
    m_nfound = 0;

    m_pWmStruct = wmNew();
}


wm_wrapper::~wm_wrapper(void)
{
    if (m_pWmStruct) 
    {
        wmFree(m_pWmStruct);
    }
}

void wm_wrapper::AddPattern(const char* pszPatten, int iLen)
{
    _ASSERT(pszPatten);
    wmAddPattern(m_pWmStruct, (unsigned char *) pszPatten, iLen);
}

void wm_wrapper::AddPattern(const char* pszPatten)
{
    _ASSERT(pszPatten);
    wmAddPattern(m_pWmStruct, (unsigned char *) pszPatten, (int) strlen(pszPatten));
}

void wm_wrapper::BuildContext()
{
    wmPrepPatterns(m_pWmStruct);
}

void wm_wrapper::Search(const char* pszSearch, int nLen)
{
    _ASSERT(pszSearch);
    m_vecResult.clear();

    wmSearch(m_pWmStruct, (unsigned char *) pszSearch, (int) nLen);
}

bool wm_wrapper::GetResult(std::vector<std::string>& vecResult)
{
    vecResult = std::move(m_vecResult);
    return true;
}


wm_wrapper::WM_STRUCT * wm_wrapper::wmNew()
{
    WM_STRUCT *p = (WM_STRUCT *) malloc(sizeof(WM_STRUCT));
    if (!p) {
        return 0;
    }
    memset(p, 0, sizeof(*p));
    p->msNumPatterns = 0;
    p->msSmallest = 1000;
    return p;
}
void wm_wrapper::wmFree(WM_STRUCT *ps)
{
    if (ps->plist) 
    {
        auto pTmp = ps->plist;
        int i = ps->msNumPatterns;
        while ( i > 0) 
        {
            pTmp = ps->plist;
            ps->plist = ps->plist->next;
            if (pTmp) 
            {
                if (pTmp->psPat) 
                {
                    free(pTmp->psPat);
                }

                

                free(pTmp);
            }

            -- i;
        }
    }

    if (ps->msPatArray)
    {
        //if (ps->msPatArray->psPat)
            //free(ps->msPatArray->psPat);
        free(ps->msPatArray);
    }
    if (ps->msNumArray)
        free(ps->msNumArray);
    if (ps->msHash)
        free(ps->msHash);
    if (ps->msPrefix)
        free(ps->msPrefix);
    if (ps->msShift)
        free(ps->msShift);
    free(ps);
}
int wm_wrapper::wmAddPattern(WM_STRUCT *ps, unsigned char *q, int m)//m字符串长度
{
    WM_PATTERN_STRUCT *p;
    p = (WM_PATTERN_STRUCT *) malloc(sizeof(WM_PATTERN_STRUCT));
    if (!p) {
        return -1;
    }
    memset(p, 0, sizeof(*p));
    p->psPat = (unsigned char*) malloc(m + 1);
    memset(p->psPat + m, 0, 1);
    memcpy(p->psPat, q, m);
    p->psLen = m;
    ps->msNumPatterns++;
    if (p->psLen < (unsigned) ps->msSmallest)
        ps->msSmallest = p->psLen;
    p->next = ps->plist;
    ps->plist = p;
    return 0;
}

unsigned wm_wrapper::HASH16(unsigned char *T)
{
    return (unsigned short) (((*T) << 8) | *(T + 1));
}

void wm_wrapper::sort(wm_wrapper::WM_STRUCT *ps)//字符串哈希值从小到大排列
{
    int m = ps->msSmallest;
    int i, j;
    unsigned char *temp;
    int flag;
    for (i = ps->msNumPatterns - 1, flag = 1; i > 0 && flag; i--)
    {
        flag = 0;
        for (j = 0; j < i; j++)
        {
            if (HASH16(&(ps->msPatArray[j + 1].psPat[m - 2])) < HASH16(
                &(ps->msPatArray[j].psPat[m - 2])))
            {
                flag = 1;
                temp = ps->msPatArray[j + 1].psPat;
                ps->msPatArray[j + 1].psPat = ps->msPatArray[j].psPat;
                ps->msPatArray[j].psPat = temp;
            }
        }
    }
}

void wm_wrapper::wmPrepHashedPatternGroups(WM_STRUCT *ps)//计算有多少个不同哈希值，且从小到大
{
    unsigned sindex, hindex, ningroup;
    int i;
    int m = ps->msSmallest;
    ps->msNumHashEntries = HASHTABLESIZE;
    ps->msHash = (HASH_TYPE*) malloc(sizeof(HASH_TYPE) * ps->msNumHashEntries);
    if (!ps->msHash)
    {
        printf("No memory in wmPrepHashedPatternGroups()\n");
        return;
    }

    for (i = 0; i < (int) ps->msNumHashEntries; i++)
    {
        ps->msHash[i] = (HASH_TYPE) -1;
    }

    for (i = 0; i < ps->msNumPatterns; i++)
    {
        hindex = HASH16(&ps->msPatArray[i].psPat[m - 2]);
        sindex = ps->msHash[hindex] = i;
        ningroup = 1;
        while ((++i < ps->msNumPatterns) && (hindex == HASH16(
            &ps->msPatArray[i].psPat[m - 2])))
            ningroup++;
        ps->msNumArray[sindex] = ningroup;
        i--;
    }
}

void wm_wrapper::wmPrepShiftTable(WM_STRUCT *ps)//建立shift表
{
    int i;
    unsigned short m, k, cindex;
    unsigned shift;
    m = (unsigned short) ps->msSmallest;
    ps->msShift = (unsigned char*) malloc(SHIFTTABLESIZE * sizeof(char));
    if (!ps->msShift)
        return;

    for (i = 0; i < SHIFTTABLESIZE; i++)
    {
        ps->msShift[i] = (unsigned) (m - 2 + 1);
    }

    for (i = 0; i < ps->msNumPatterns; i++)
    {
        for (k = 0; k < m - 1; k++)
        {
            shift = (unsigned short) (m - 2 - k);
            cindex = ((ps->msPatArray[i].psPat[k] << 8)
                | (ps->msPatArray[i].psPat[k + 1]));//B为2
            if (shift < ps->msShift[cindex])
                ps->msShift[cindex] = shift;//k=m-2时，shift=0，
        }
    }
}

void wm_wrapper::wmPrepPrefixTable(WM_STRUCT *ps)//建立Prefix表
{
    int i;
    ps->msPrefix = (HASH_TYPE*) malloc(sizeof(HASH_TYPE) * ps->msNumPatterns);
    if (!ps->msPrefix)
    {
        printf("No memory in wmPrepPrefixTable()\n");
        return;
    }
    for (i = 0; i < ps->msNumPatterns; i++)
    {
        ps->msPrefix[i] = HASH16(ps->msPatArray[i].psPat);
    }
}

void wm_wrapper::wmGroupMatch(WM_STRUCT *ps,//后缀哈希值相同，比较前缀以及整个字符匹配
    int lindex, unsigned char *Tx, unsigned char *T)
{
    WM_PATTERN_STRUCT *patrn;
    WM_PATTERN_STRUCT *patrnEnd;
    int text_prefix;
    unsigned char *px, *qx;

    patrn = &ps->msPatArray[lindex];
    patrnEnd = patrn + ps->msNumArray[lindex];
    text_prefix = HASH16(T);

    for (; patrn < patrnEnd; patrn++)
    {
        if (ps->msPrefix[lindex++] != text_prefix)
        {
            continue;
        }
        else
        {
            px = patrn->psPat;
            qx = T;
            while (*(px++) == *(qx++) && *(qx - 1) != '\0')
                ;

            if (*(px - 1) == '\0')
            {
                //printf("Match pattern \"%s\" at line %d column %d\n",
                //    patrn->psPat, m_nline, T - Tx + 1);

                m_nfound++;

                m_vecResult.push_back(std::string((char*)patrn->psPat));
            }
        }
    }
}

int wm_wrapper::wmPrepPatterns(WM_STRUCT *ps)//由plist得到msPatArray
{
    int kk;
    WM_PATTERN_STRUCT *plist;

    ps->msPatArray = (WM_PATTERN_STRUCT*) malloc(sizeof(WM_PATTERN_STRUCT)
        * ps->msNumPatterns);
    if (!ps->msPatArray)
        return -1;
    ps->msNumArray
        = (unsigned short*) malloc(sizeof(short) * ps->msNumPatterns);
    if (!ps->msNumArray)
        return -1;

    for (kk = 0, plist = ps->plist; plist != NULL && kk < ps->msNumPatterns; plist
        = plist->next)
    {
        memcpy(&ps->msPatArray[kk++], plist, sizeof(WM_PATTERN_STRUCT));
    }
    sort(ps);
    wmPrepHashedPatternGroups(ps);
    wmPrepShiftTable(ps);
    wmPrepPrefixTable(ps);
    return 0;
}

void wm_wrapper::wmSearch(WM_STRUCT *ps, unsigned char *Tx, int n)//字符串查找
{
    int Tleft, lindex, tshift;
    unsigned char *T, *Tend, *window;
    Tleft = n;
    Tend = Tx + n;
    if (n < ps->msSmallest)
        return;
    for (T = Tx, window = Tx + ps->msSmallest - 1; window < Tend; T++, window++, Tleft--)
    {
        tshift = ps->msShift[(*(window - 1) << 8) | *window];
        while (tshift)
        {
            window += tshift;
            T += tshift;
            Tleft -= tshift;
            if (window > Tend)
                return;
            tshift = ps->msShift[(*(window - 1) << 8) | *window];
        }
        if ((lindex = ps->msHash[(*(window - 1) << 8) | *window])
            == (HASH_TYPE) -1)
            continue;
        lindex = ps->msHash[(*(window - 1) << 8) | *window];
        wmGroupMatch(ps, lindex, Tx, T);
    }
}

