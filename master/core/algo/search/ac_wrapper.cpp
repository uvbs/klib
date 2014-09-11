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

void ac_wrapper::set_callback(ac_match_callback call)
{
    callback_ = call;
}

bool ac_wrapper::search(const char* buf, int buflen)
{
    int cur_state = 0;

    unsigned ret = bnfaSearch((bnfa_struct_t*)handle_,
        (unsigned char*)buf,
        buflen,
        Match,    // match func
        this,   // data
        0,      // sindex
        &cur_state);

    bnfa_struct_t* ptr = (bnfa_struct_t*)handle_;

    return true;
}

int ac_wrapper::Match(void * id, void *tree, int index, void *data, void *neg_list)
{
    ac_wrapper* wrap = (ac_wrapper*) data; 
    if (wrap->callback_) {
        wrap->callback_(id, tree, index, neg_list);
    }
    return 0;
}