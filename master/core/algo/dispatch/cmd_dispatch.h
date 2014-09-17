#ifndef _cmd_dispatch_h_
#define _cmd_dispatch_h_


#include "../search/ac_wrapper.h"
#include "../search/wm_wrapper.h"
using namespace klib::algo;


typedef  int (*dispatch_handler_func)(void* handle_data, 
                    const char* buff_ptr, 
                    size_t buf_len, 
                    size_t index, 
                    matched_patten* patt);

class cmd_dispatch;
struct dispatch_ctx
{
    cmd_dispatch* dispatcher_;
    const char*   buff_ptr_;
    size_t        buff_len_;
    void*         handle_data_;
};

// dispatcher
class cmd_dispatch
{
public:
    cmd_dispatch(void);
    ~cmd_dispatch(void);
    
    enum e_process_mode
    {
        e_process_begin,        // 命令是开始位置
        e_process_any,          // 命令是任意位置
    };

public:
    bool add_handler(const char* str_cmd, 
        size_t cmd_len,
        dispatch_handler_func f);
    void complete();

    bool process(const char* buff, 
        size_t len, 
        e_process_mode mod = e_process_begin,
        void* handle_data  = nullptr);

protected:
    static int search_handler(
        void* ctx,
        void* user_data, 
        matched_patten*, 
        int index);

protected:
    ac_wrapper  ac_;
    size_t      max_patt_len_;
};



#endif // _cmd_dispatch_h_