#ifndef  _klib_ac_wrapper_h_
#define _klib_ac_wrapper_h_


#include "snort/bnfa_search.h"
#include "../common/algo_def.h"


namespace klib {
namespace algo {
    
// wrapper for ac multi pattern 
class ac_wrapper
{
public:
    ac_wrapper() ;
    ~ac_wrapper() ;

    bool add_pattern(const char* pt_buf, int   pt_len, void* data);
    bool add_pattern(const char* pt_buf, void* data);
    bool compile();
    int  size();

    void set_callback(search_match_callback call, void* pcall_this);
    bool search(const char* buf, int buflen);

protected:
    static int Match(void * user_data, 
        bnfa_pattern_t*, 
        void *tree, 
        int index, 
        void *data, 
        void *neg_list);

protected:
    void*              handle_;

    void*                  callback_ctx_;
    search_match_callback  callback_func_;
};



}}



#endif // _klib_ac_wrapper_h_