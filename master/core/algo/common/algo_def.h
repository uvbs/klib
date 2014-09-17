#ifndef _algo_def_h_
#define _algo_def_h_


struct matched_patten
{
    char*   pstr;
    size_t  len;
};

typedef int (*search_match_callback)(void* ctx,
    void * user_data,
    matched_patten*, 
    int index);


#endif //_algo_def_h_ 