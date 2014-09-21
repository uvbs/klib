#include "kr.h"

#define REHASH(a, b, h) ((((h)- (a) * d) << 1) + (b))
 
int krap_rabin(const char *text, const char *pat)
{
    int j = 0, d= 1;
    int text_h = 0, pat_h = 0;
    int n, m;
    n = strlen(text);
    m = strlen(pat);
    d = d << (m -1);
    for(; j < m ; j++){
        pat_h = ((pat_h << 1) + pat[j]);
        text_h = ((text_h << 1) + text[j]);
    }
 
    j = 0;
    while(j < n - m) {
        if(pat_h == text_h)
            printf("find %s in [%d]\n", pat ,j);
 
        text_h = REHASH(text[j], text[j + m], text_h );
        j++;
    }
 
    return 0;
 
}