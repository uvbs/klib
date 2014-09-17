#include "cmd_dispatch.h"


cmd_dispatch::cmd_dispatch(void)
    : max_patt_len_(0)
{
}

cmd_dispatch::~cmd_dispatch(void)
{
}

bool cmd_dispatch::add_handler(const char* str_cmd, 
    size_t cmd_len,
    dispatch_handler_func f)
{
    ac_.add_pattern(str_cmd, cmd_len, f);

    if (cmd_len > max_patt_len_)
        max_patt_len_ = cmd_len;

    return true;
}

void cmd_dispatch::complete()
{
    ac_.compile();
}

bool cmd_dispatch::process(
    const char* buff, 
    size_t len, 
    e_process_mode mod,
    void* handle_data)
{
    dispatch_ctx ctx;
    ctx.dispatcher_ = this;
    ctx.buff_ptr_   = buff;
    ctx.buff_len_   = len;
    ctx.handle_data_ = handle_data;

    ac_.set_callback(&cmd_dispatch::search_handler, &ctx);

    if (e_process_begin == mod)
        return ac_.search(buff, max_patt_len_);
    else
        return ac_.search(buff, len);
}

int cmd_dispatch::search_handler(
    void* ctx,
    void* user_data, 
    matched_patten* pattern, 
    int index)
{
    dispatch_ctx* ctx_ptr = (dispatch_ctx*)ctx;

    dispatch_handler_func func = (dispatch_handler_func) user_data;
    if (func)
    {
        func(ctx_ptr->handle_data_, 
            ctx_ptr->buff_ptr_,
            ctx_ptr->buff_len_,
            index,
            pattern);
    }

    // ²»ÔÙ¼ÌĞøËÑË÷
    return 1;
}