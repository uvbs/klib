
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
    e_http_get = 1,
    e_http_post,
    e_http_default = e_http_get,
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

typedef std::function<void(const char* data, size_t len)> data_callback;

class trunk_reader
{
public:
    trunk_reader(tcp_socket& sock)
        : sock_(sock)
        , read_err_(false)
    {
    }

    bool read(const data_callback&);
    bool is_error();

protected:
    bool read(char* buf, size_t len);

protected:
    tcp_socket& sock_;

    bool        read_err_;
};

class http 
{
public:
    http() {}
    virtual ~http() {}
    
    enum chunk_parser_result
    {
        e_parse_error,
        e_parse_continue,
        e_parse_ok,
    };

    enum e_content_data_type
    {
        e_content_length,
        e_content_chunk,
        e_content_close,
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
    e_content_data_type judge_data_type(const char* buff, size_t buf_len, size_t& data_len);
    bool handle_close_response(char*, const data_callback& handler);
    bool handle_chunk_response(char*, const data_callback& handler);
    bool handle_content_response(char*, const data_callback& handler, size_t content_len);

    template<typename CharType>
    CharType* skip_space(CharType*& );

protected:
    tcp_socket              socket_;                      // TCP套接字对象
    request_header_info     header_info_;

    std::string             stats_url_;
    e_http_method           http_method_;

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
