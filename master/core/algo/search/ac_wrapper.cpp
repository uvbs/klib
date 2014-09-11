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

bool ac_wrapper::add_pattern(char* pt_buf, int pt_len, void* data)
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

bool ac_wrapper::search(const char* buf, int buflen)
{
    int cur_state = 0;

    unsigned ret = bnfaSearch((bnfa_struct_t*)handle_,
        (unsigned char*)buf,
        buflen,
        nullptr,
        this,
        0,
        &cur_state);
    return true;
}