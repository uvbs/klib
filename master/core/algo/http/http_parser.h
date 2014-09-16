#ifndef _http_parser_h_
#define _http_parser_h_

#include "../search/ac_wrapper.h"
#include "../search/wm_wrapper.h"
using namespace klib::algo;

#include "protocol_define.h"
#include "../common/sstring.h"


enum e_http_method
{
    e_http_unknow,
    e_http_get,
    e_http_post,
    e_http_put,
    e_http_delete,
};

// define packet structure
struct http_packet
{
public:
    char*       buff;
    size_t      len;

    e_http_method   method_;
    size_t          content_length_;

    sstring     s_get_;           // request
    sstring     s_post_;          // for post    pass=&name=

    sstring     s_host_;          // for host
    sstring     s_accept_;        // for accept
    sstring     s_cookie_;
    sstring     s_content_type_;  // for content-type
    sstring     s_content_length_;

    sstring     s_body_;          // 后面的内容
};

typedef void (*extract_func_type)(http_packet*, sstring &s, char* buff, size_t buf_len, size_t index, matched_patten*);

char* skip_space(char*& p);

void extract_method_get_func(http_packet*, sstring &s, char* buff, size_t buf_len, size_t index, matched_patten*);
void extract_method_post_func(http_packet*, sstring &s, char* buff, size_t buf_len, size_t index, matched_patten*);
void extract_method_content_len_func(http_packet*, sstring &s, char* buff, size_t buf_len, size_t index, matched_patten*);

void extract_until_line(http_packet*, sstring &s, char* buff, size_t buf_len, size_t index, matched_patten*);
void extract_until_space(http_packet*, sstring &s, char* buff, size_t buf_len, size_t index, matched_patten*);
void extract_until_end(http_packet*, sstring &s, char* buff, size_t buf_len, size_t index, matched_patten*);

char* skip_until_space(char* p);
char* skip_until_line(char* p);

enum http_header_type
{
    e_http_,
};

struct http_header_ctx
{
    http_header_ctx()
        : str(nullptr)
        , pattern_len(0)
    {
    }

    sstring*            str;
    size_t              pattern_len;
    extract_func_type   extrac_func;
};


// http protocol parser 
class http_parser
{
public:
    void init();
    bool process(http_packet*);

protected:
    static int search_handler(
        void* pthis,
        void * user_data, 
        matched_patten*, 
        int index);

protected:
    wm_wrapper ac_;
    
    http_packet* pkt_;
    
    http_header_ctx  get_ctx_;
    http_header_ctx  post_ctx_;
    
    http_header_ctx  host_ctx_;
    http_header_ctx  accept_ctx_;
    http_header_ctx  cookie_ctx_;
    http_header_ctx  content_type_ctx_;
    http_header_ctx  content_length_ctx_;

    http_header_ctx  body_ctx_;
};




#endif // _http_parser_h_