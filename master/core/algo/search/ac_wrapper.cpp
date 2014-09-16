#include "ac_wrapper.h"

#include "snort/bnfa_search.h"
#include "snort/bnfa_search.c"

using namespace klib::algo;

ac_wrapper::ac_wrapper() 
    : handle_(nullptr)
{
    handle_ = (bnfa_struct_t*) bnfaNew(nullptr, nullptr, nullptr);

}

ac_wrapper::~ac_wrapper() 
{
    if (handle_) {
        bnfaFree((bnfa_struct_t*) handle_);
        handle_ = nullptr;
    }
}

bool ac_wrapper::add_pattern(const char* pt_buf, int pt_len, void* data)
{
    int ret = bnfaAddPattern( (bnfa_struct_t *) handle_,
        (unsigned char*) pt_buf, pt_len, false,
        false, data);

    return 0 == ret;
}

bool ac_wrapper::add_pattern(const char* pt_buf, void*data)
{
    int ret = bnfaAddPattern( (bnfa_struct_t *) handle_,
        (unsigned char*) pt_buf, strlen(pt_buf), false,
        false, data);

    return 0 == ret;
}

bool ac_wrapper::compile()
{
    int ret = bnfaCompile( (bnfa_struct_t *) handle_,
        nullptr,
        nullptr);

    return 0 == ret;
}

int  ac_wrapper::size()
{
    return bnfaPatternCount((bnfa_struct_t *) handle_);
}

void ac_wrapper::set_callback(search_match_callback call, void* pthis)
{
    callback_func_ = call;
    callback_this_ = pthis;
}

bool ac_wrapper::search(const char* buf, int buflen)
{
    int cur_state = 0;

    unsigned ret = bnfaSearch((bnfa_struct_t*)handle_,
        (unsigned char*)buf,
        buflen,     // buffer length
        Match,      // match func
        this,       // data
        0,          // sindex
        &cur_state);
    
    return 1 == ret;
}

int ac_wrapper::Match(void * user_data, 
    bnfa_pattern_t* patrn, 
    void *tree, 
    int index, 
    void *data, 
    void *neg_list)
{
    ac_wrapper* wrap = (ac_wrapper*) data; 
    if (wrap->callback_func_) {
        matched_patten patten;
        patten.len  = patrn->n;
        patten.pstr = (char*) patrn->casepatrn;
        return wrap->callback_func_(wrap->callback_this_, user_data, &patten, index);
    }
    return 0;
}