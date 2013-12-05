#pragma once

#include <stdlib.h>
#include <string>
#include <ctype.h>

namespace klib{
namespace algo{



/**
 * bm 模式匹配算法封装
 */
class bm_wrapper
{
public:
    bm_wrapper(void)
    {
        memset(&m_hbm, 0, sizeof(m_hbm));
    }

    bm_wrapper::~bm_wrapper(void)
    {
        if (m_hbm.Pnc) 
        {
            free(m_hbm.Pnc);
            m_hbm.Pnc = NULL;
        }
    }

public:
    /**
     * @brief  初始化被查找的字符串模式
     */
    void init_pattern(unsigned char* pszPatten, int nLen, bool bCaseSense)
    {
        if (m_hbm.Pnc) 
        {
            free(m_hbm.Pnc);
            m_hbm.Pnc = NULL;
        }

        hbm_prepx(&m_hbm, pszPatten, nLen, ! bCaseSense);
    }

    /**
     * @brief 查找字符串
     */
    int search(const unsigned char *buf, int blen)
    {
        auto pPos = hbm_match(&m_hbm, buf, blen);
        if (pPos) 
        {
            return (int)(pPos - buf);
        }
        return -1;
    }

private:

    typedef struct 
    {
        unsigned char *P;
        unsigned char *Pnc;
        int            M;
        int            bcShift[256];
        int            nocase;
    }HBM_STRUCT;

    HBM_STRUCT m_hbm;

protected:

//     HBM_STATIC HBM_STRUCT    * hbm_prep( unsigned char * pat, int m, int nocase );
//     HBM_STATIC int hbm_prepx( HBM_STRUCT *p, unsigned char * pat, int m, int nocase );
//     HBM_STATIC const unsigned char * hbm_match( HBM_STRUCT *p, const unsigned char * text, int n );
//     HBM_STATIC void            hbm_free( HBM_STRUCT *p );


    int bm_wrapper::hbm_prepx (HBM_STRUCT *p, unsigned char * pat, int m, int nocase )
    {
        int            i,k;
        unsigned char *t;
        if( !m ) return 0;
        if( !p ) return 0;

        p->P = pat;
        p->M = m;
        p->nocase = nocase;

        if( nocase ) /* convert to uppercase */
        {
            t = (unsigned char*)malloc(m);
            if ( !t ) return 0;

            memcpy(t,pat,m);

            for(i=0;i<m;i++)
            {
                t[i] = (unsigned char)toupper(t[i]);
            }
            p->Pnc = t;
        }
        else
        {
            p->Pnc = 0;
        }

        /* Compute normal Boyer-Moore Bad Character Shift */
        for(k = 0; k < 256; k++) p->bcShift[k] = m;

        if( nocase )
        {
            for(k = 0; k < m; k++)
                p->bcShift[ p->Pnc[k] ] = m - k - 1;
        }
        else
        {
            for(k = 0; k < m; k++)
                p->bcShift[ p->P[k] ] = m - k - 1;
        }

        return 1;
    }

    /*
     *
     */
    bm_wrapper::HBM_STRUCT * bm_wrapper::hbm_prep(unsigned char * pat, int m, int nocase)
    {
        HBM_STRUCT    *p;

        p = (HBM_STRUCT*)malloc(sizeof(HBM_STRUCT));
        if (!p)
        {
            //DynamicEngineFatalMessage("Failed to allocate memory for pattern matching.");
        }

        if( !hbm_prepx( p, pat, m, nocase) )
        {
            //DynamicEngineFatalMessage("Error initializing pattern matching. Check arguments.");
        }

        return p;
    }

    /*
    *
    */
    void bm_wrapper::hbm_free( HBM_STRUCT *p )
    {
        if(p)
        {
            if( p->Pnc )free(p->Pnc);
            free(p);
        }
    }

    /*
    *   Boyer-Moore Horspool
    *   Does NOT use Sentinel Byte(s)
    *   Scan and Match Loops are unrolled and separated
    *   Optimized for 1 byte patterns as well
    *
    */
    const unsigned char * bm_wrapper::hbm_match(HBM_STRUCT * px, const unsigned char * text, int n)
    {
        const unsigned char *pat, *t, *et, *q;
        int            m1, k;
        int           *bcShift;

        if( px->nocase  )
        {
            pat = px->Pnc;
        }
        else
        {
            pat = px->P;
        }
        m1     = px->M-1;
        bcShift= px->bcShift;

        //printf("bmh_match: pattern=%.*s, %d bytes \n",px->M,pat,px->M);

        t  = text + m1;
        et = text + n;

        /* Handle 1 Byte patterns - it's a faster loop */
        if( !m1 )
        {
            if( !px->nocase  )
            {
                for( ;t<et; t++ )
                    if( *t == *pat ) return t;
            }
            else
            {
                for( ;t<et; t++ )
                    if( toupper(*t) == *pat ) return t;
            }
            return 0;
        }

        if( !px->nocase )
        {
            /* Handle MultiByte Patterns */
            while( t < et )
            {
                /* Scan Loop - Bad Character Shift */
                do
                {
                    t += bcShift[*t];
                    if( t >= et )return 0;;

                    t += (k=bcShift[*t]);
                    if( t >= et )return 0;

                } while( k );

                /* Unrolled Match Loop */
                k = m1;
                q = t - m1;
                while( k >= 4 )
                {
                    if( pat[k] != q[k] )goto NoMatch;  k--;
                    if( pat[k] != q[k] )goto NoMatch;  k--;
                    if( pat[k] != q[k] )goto NoMatch;  k--;
                    if( pat[k] != q[k] )goto NoMatch;  k--;
                }
                /* Finish Match Loop */
                while( k >= 0 )
                {
                    if( pat[k] != q[k] )goto NoMatch;  k--;
                }
                /* If matched - return 1st char of pattern in text */
                return q;

NoMatch:
                t++;
            }

        }
        else  /* NoCase - convert input string to upper case as we process it */
        {

            /* Handle MultiByte Patterns */
            while( t < et )
            {
                /* Scan Loop - Bad Character Shift */
                do
                {
                    t += bcShift[toupper(*t)];
                    if( t >= et )return 0;;

                    t += (k=bcShift[toupper(*t)]);
                    if( t >= et )return 0;

                } while( k );

                /* Unrolled Match Loop */
                k = m1;
                q = t - m1;
                while( k >= 4 )
                {
                    if( pat[k] != toupper(q[k]) )goto NoMatchNC;  k--;
                    if( pat[k] != toupper(q[k]) )goto NoMatchNC;  k--;
                    if( pat[k] != toupper(q[k]) )goto NoMatchNC;  k--;
                    if( pat[k] != toupper(q[k]) )goto NoMatchNC;  k--;
                }
                /* Finish Match Loop */
                while( k >= 0 )
                {
                    if( pat[k] != toupper(q[k]) )goto NoMatchNC;  k--;
                }
                /* If matched - return 1st char of pattern in text */
                return q;

NoMatchNC:
                t++;
            }

        }

        return 0;
    }

};



}}
