#include "http_parser.h"


void extract_method_get_func(http_packet* ptk, sstring &s, char* buff, size_t buf_len, size_t index, matched_patten* patrn)
{
    s.ptr = buff + index + patrn->len;
    s.ptr = skip_space(s.ptr);
    
    char* end = skip_until_space(s.ptr);
    s.len = end - s.ptr;

    ptk->method_ = e_http_get;
}

void extract_method_post_func(http_packet* ptk, sstring &s, char* buff, size_t buf_len, size_t index, matched_patten* patrn)
{
    s.ptr = buff + index + patrn->len;
    s.ptr = skip_space(s.ptr);
    
    char* end = skip_until_space(s.ptr);
    s.len = end - s.ptr;

    ptk->method_ = e_http_post;
}

void extract_method_content_len_func(http_packet* pkt, sstring &s, char* buff, size_t buf_len, size_t index, matched_patten* patrn)
{
    s.ptr = buff + index + patrn->len;
    s.ptr = skip_space(s.ptr);
    
    char* end = skip_until_space(s.ptr);
    s.len = end - s.ptr;

    pkt->content_length_ = strtoul(s.ptr, nullptr, 10);
}

void extract_until_line(http_packet*, sstring &s, char* buff, size_t buf_len, size_t index, matched_patten* patrn)
{
    s.ptr = buff + index + patrn->len;
    s.ptr = skip_space(s.ptr);
    
    char* end = skip_until_line(s.ptr);
    s.len = end - s.ptr;
}

void extract_until_space(http_packet*, sstring &s, char* buff, size_t buf_len, size_t index, matched_patten* patrn)
{
    s.ptr = buff + index + patrn->len;
    s.ptr = skip_space(s.ptr);

    char* end = skip_until_space(s.ptr);
    s.len = end - s.ptr;
}

void extract_until_end(http_packet*, sstring &s, char* buff, size_t buf_len, size_t index, matched_patten* patrn)
{
    s.ptr = buff + index + patrn->len;
    s.len = (buff + buf_len) - s.ptr;
}

char* skip_space(char*& p)
{
    while (*p && *p <= 0x20) 
    {
        // 回车换行不跳过
        if (*p == '\n' || *p == '\r') 
        {
            break;
        }
        ++ p;
    }

    return p;
}

size_t parsed_line(char* p)
{
    if (*p == '\n') 
    {
        return 1;
    }
    if (*p == '\r' && *(p + 1) == '\n') 
    {
        return 2;
    }
    return 0;
}

char* skip_until_line(char* p)
{
    size_t len;
    while (*p && (len = parsed_line(p)) == 0)
    {
        ++ p;
    }

    return p;
}

char* skip_until_space(char* p)
{
    while (*p && *p != ' ')
    {
        ++ p;
    }

    return p;
}

void http_parser::init()
{
    get_ctx_.extrac_func  = extract_method_get_func;
    post_ctx_.extrac_func = extract_method_post_func;

    host_ctx_.extrac_func           = extract_until_line;
    accept_ctx_.extrac_func         = extract_until_line;
    cookie_ctx_.extrac_func         = extract_until_line;
    content_type_ctx_.extrac_func   = extract_until_line;
    content_length_ctx_.extrac_func = extract_method_content_len_func;
    body_ctx_.extrac_func           = extract_until_end;
    
    ac_.add_pattern(HTTP_GET, &get_ctx_);
    ac_.add_pattern(HTTP_POST, &post_ctx_);

    ac_.add_pattern(HTTP_HOST, &host_ctx_);
    ac_.add_pattern(HTTP_ACCEPT, &accept_ctx_);
    ac_.add_pattern(HTTP_COOKIE,  &cookie_ctx_);
    ac_.add_pattern(HTTP_Content_Type, &content_type_ctx_);
    ac_.add_pattern(HTTP_Content_Length, &content_length_ctx_);

    ac_.add_pattern(HTTP_BODY, &body_ctx_);

    ac_.compile();
}

bool http_parser::process(http_packet* pkt)
{
    pkt_ = pkt;

    get_ctx_.str = &pkt->s_get_;
    post_ctx_.str      = &pkt->s_post_;

    host_ctx_.str = &pkt->s_host_;
    accept_ctx_.str = &pkt->s_accept_;
    cookie_ctx_.str  = &pkt->s_cookie_;
    content_type_ctx_.str = &pkt->s_content_type_;
    content_length_ctx_.str = &pkt->s_content_length_;

    body_ctx_.str          = &pkt->s_body_;

    ac_.set_callback(&http_parser::search_handler,
        this);
    ac_.search(pkt->buff, pkt->len);
    
    return true;
}

int http_parser::search_handler(
    void* p,
    void * user_data, 
    matched_patten* patrn,
    int index)
{
    http_parser* pthis = (http_parser*)p;

    http_header_ctx* ext = (http_header_ctx*) user_data; 
    if (ext) {
        ext->extrac_func(pthis->pkt_,
            *ext->str, 
            pthis->pkt_->buff, 
            pthis->pkt_->len, 
            index, 
            patrn);
    }

    return 0;
}


