
#if !defined(_klib_http_H)
#define _klib_http_H

#ifdef _WIN32
#include <WinSock2.h>
#include <windows.h>
#endif

#include <functional>
#include <cstdio>
#include <string>
#include <vector>

#include "../tcp_socket.h"


namespace klib {
namespace net  {


///< 定义下载接收缓冲区大小
#define  HTTPDOWN_RECV_BUFF     (4*1024)


// http提交的方式
enum e_http_method
{
    e_stats_get = 1,
    e_stats_post,
    e_stats_default = e_stats_get,
};

// 下载时的头部的一些信息
struct request_header_info
{
    std::string cookie_;
    std::string refer_;
    std::string user_agent_;
};

typedef std::pair<std::string, std::string> pair_type;
typedef std::vector<pair_type> http_param_list;
typedef std::vector<pair_type> http_header_list;


class http 
{
public:
    http() {}
    virtual ~http() {}

    typedef std::function<void(const char* data, size_t len)> data_callback;

    enum chunk_parser_result
    {
        e_parse_error,
        e_parse_continue,
        e_parse_ok,
    };
public:
    void set_url(const std::string& url);
    void set_stats_method(e_http_method method);
    void add_header(const std::string& key, const std::string& val);
    void add_post_param(const std::string& key, const std::string& val);

    ///< 解析URL地址
    bool parse_url(const char* url, char* hostAddr, int& port, char* getPath) ;

    bool get_content(const std::string& url, std::string& content);
    bool download(const std::string& url, const std::string& path);

protected:
    // 发送请求
    bool handle_requst(const std::string url, const data_callback& handler);
    bool send_request(const std::string url);
    bool handle_chunk_response(char*, const data_callback& handler);
    bool handle_content_response(char*, const data_callback& handler, size_t content_len);

    chunk_parser_result parser_chunk_info(char* buff, 
        size_t buf_len,
        size_t& data_size,
        char*&  chunk_buff,
        size_t& chunk_left,
        size_t& extra_length
    );

    template<typename CharType>
    CharType* skip_space(CharType*& );

protected:
    tcp_socket              socket_;                      // TCP套接字对象
    request_header_info     header_info_;

    std::string             stats_url_;
    e_http_method           stats_method_;

    http_header_list        http_header_list_;
    http_param_list         http_post_param_lst_;
};


template<typename CharType>
CharType* http::skip_space(CharType*& buf)
{
    while (isspace(*buf))
    {
        buf ++;
    }

    return buf;
}



}}



#endif // !defined(_klib_http_H)
