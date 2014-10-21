// HttpDwonload.cpp: implementation of the http class.
//
//////////////////////////////////////////////////////////////////////

#include "http.h"
#include "../addr_resolver.h"
#include "../../core/scope_guard.h"

#pragma warning(disable: 4996)

using namespace klib::core;
using namespace klib::net;


#define default_user_agent "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.17 (KHTML, like Gecko) Chrome/24.0.1312.52 Safari/537.17"


bool trunk_reader::read(const data_callback& func)
{
    char tmp_buf[1024];
    char buf[4];

    do 
    {
        char* cur_read_pos = tmp_buf;

        while(this->read(buf, 1))
        {
            *cur_read_pos = buf[0];
            cur_read_pos ++;

            if (buf[0] == '\n') 
            {
                *cur_read_pos = '\0';
                break;
            }
        }

        int trunk_len = 0;
        int scan_ret = sscanf(tmp_buf, "%x", &trunk_len);
        if (scan_ret < 0) {
            read_err_ = true;
            return false;
        }
        if (0 == trunk_len) {
            return true;
        }

        size_t left_trunk_len = trunk_len;
        while (left_trunk_len > 0)
        {
            size_t to_read = std::min<size_t>(left_trunk_len, sizeof(tmp_buf) - 1);

            if (this->read(tmp_buf, to_read))
            {
                func(tmp_buf, to_read);
                left_trunk_len -= to_read;
            }
            else 
            {
                read_err_ = true;
                return false;
            }
        }

        // 读取掉回车换行
        this->read(tmp_buf, 2);
    } while (true);

    return true;
}

bool trunk_reader::is_error()
{
    return read_err_;
}

bool trunk_reader::read(char* buf, size_t len)
{
    size_t read_left = len;

    char* cur_pos = buf;
    while (read_left > 0)
    {
        int ret = sock_.recv(cur_pos, read_left, 0, 0);
        if (ret < 0) {
            return false;
        }

        read_left -= ret;
    }

    return true;
}

//----------------------------------------------------------------------
// 
bool http::parse_url(const char* url, char* hostAddr, int& port, char* getPath) 
{
    if (url == NULL || hostAddr == NULL || getPath == NULL) 
    {
        return false;
    }

    const char* temp = strstr(url, "http://");
    if (temp == NULL) 
    {
        return false;
    }
    port = 80;
    const char* hostStart = temp + strlen("http://");
    const char* colon = strchr(hostStart, ':');
    if (colon != NULL)    //表示存在冒号,有端口号
    {
        sscanf(hostStart, "%[^:]:%d%s", hostAddr, &port, getPath);
    }
    else
    {
        sscanf(hostStart, "%[^/]%s", hostAddr, getPath);
    }

    return true;
}

void http::set_url(const std::string& url)
{
    stats_url_ = url;
}

void http::set_stats_method(e_http_method method)
{
    http_method_ = method;
}

void http::add_header(const std::string& key, const std::string& val)
{
    pair_type p;
    p.first  = key;
    p.second = val;
    http_header_list_.push_back(std::move(p));
}

void http::add_post_param(const std::string& key, const std::string& val)
{
    pair_type p;
    p.first  = key;
    p.second = val;
    http_post_param_lst_.push_back(std::move(p));
}

bool http::get_content(const std::string& http_url, std::string& str_content)
{
    data_callback callback = [&](const char* data, size_t len)
    {
        str_content.append(data, len);
    };

    bool result = handle_requst(http_url, callback);
    if (!result) {
        str_content.clear();
    }

    return result;
}

bool http::download(const std::string& http_url, const std::string& path)
{
    FILE* hFile = fopen(path.c_str(), "wb");    //创建文件
    if (nullptr == hFile) {
        return false;
    }
    defer ( fclose(hFile) );

    data_callback callback = [&](const char* data, size_t len)
    {
        fwrite(data, len, 1, hFile);
    };

    return handle_requst(http_url, callback);
}

bool http::send_request(const std::string http_url)
{
    if (http_url.empty()) {
        return false;
    }

    char szHostName[MAX_PATH] = {0};
    char szGet[MAX_PATH] = {0};
    int  port = 0;
    if (!parse_url(http_url.c_str(), szHostName, port, szGet))
    {
        return false;
    }

    DWORD dwRetryTime = 0;
    socket_.init();
    while (!socket_.connect(szHostName, port)) 
    {
        ++ dwRetryTime;
        if (dwRetryTime >= 4) 
        {
            return false;
        }

        Sleep((DWORD)(1000 * 2.5));
    }

    std::string request_header;
    if (e_http_get == this->http_method_) {
        request_header.append("GET ");
    }
    else {
        request_header.append("POST ");
    }
    request_header.append(szGet);
    request_header.append(" HTTP/1.1\r\n");

    request_header.append("Host: ");
    request_header.append(szHostName);
    request_header.append("\r\n");

    request_header.append("Connection: close\r\n");

    if (!header_info_.refer_.empty()) 
    {
        request_header.append("Referer: ");
        request_header.append(header_info_.refer_);
        request_header.append("\r\n");
    }

    if (!header_info_.cookie_.empty()) 
    {
        request_header.append("Cookie: ");
        request_header.append(header_info_.cookie_);
        request_header.append("\r\n");
    }

    if (!header_info_.user_agent_.empty()) 
    {
        request_header.append("User-Agent: ");
        request_header.append(header_info_.user_agent_);
        request_header.append("\r\n");
    }
    else 
    {
        request_header.append("User-Agent: ");
        request_header.append(default_user_agent);
        request_header.append("\r\n");
    }

    for (auto itr = http_header_list_.begin(); 
        itr != http_header_list_.end();
        ++ itr)
    {
        request_header.append(itr->first);
        request_header.append(": ");
        request_header.append(itr->second);
        request_header.append("\r\n");
    }

    request_header.append("\r\n");

    // 添加post请求参数
    if (e_http_post == http_method_) 
    {
        for (auto itr = http_post_param_lst_.begin(); 
            itr != http_post_param_lst_.end();
            ++ itr)
        {
            request_header.append(itr->first);
            request_header.append("=");
            request_header.append(itr->second);
            request_header.append("&");
        }
    }

    if (!socket_.send(request_header.c_str(), request_header.size())) 
    {
        return false;
    }

    return true;
}

bool http::handle_requst(const std::string http_url, const data_callback& handler)
{
    if (!send_request(http_url))
    {
        return false;
    }

    char recv_buff[HTTPDOWN_RECV_BUFF];
    DWORD dwRecvedBytes = 0;
    int iRecvRet;
    while (true) 
    {
        iRecvRet = socket_.recv(recv_buff + dwRecvedBytes, 1, 0, 0);
        if (iRecvRet <= 0) 
        {
            return false;
        }

        ++ dwRecvedBytes;
        if (dwRecvedBytes > HTTPDOWN_RECV_BUFF) 
        {
            return false;
        }

        if (dwRecvedBytes > 4) 
        {
            if (memcmp(&recv_buff[dwRecvedBytes - 4], "\r\n\r\n", 4) == 0) 
            {
                break;
            }
        }
    }
    recv_buff[dwRecvedBytes] = '\0';

    // 解析收到的数据
    if (strnicmp(recv_buff, "HTTP/", 5)) 
    {
        return false;
    }

    int status = 0; 
    float version = 0.0;
    ULONGLONG startPos, endPos, totalLength;
    startPos = endPos = totalLength = 0;
    if (sscanf(recv_buff, "HTTP/%f %d ", &version, &status) != 2) 
    {
        return false;
    }

    if (status != 200 && status != 206 ) 
    {
        return false;
    }

    size_t parsed_data_len = 0;
    e_content_data_type tp = judge_data_type(recv_buff, dwRecvedBytes, parsed_data_len);

    if (e_content_close == tp) 
    {
        return handle_close_response(recv_buff, handler);
    }
    else if (e_content_chunk == tp) 
    {
        return handle_chunk_response(recv_buff, handler);
    }
    else if (e_content_length == tp)
    {
        return handle_content_response(recv_buff, handler, parsed_data_len);
    }
    
    return false;
}

http::e_content_data_type http::judge_data_type(const char* buff, 
    size_t buf_len, 
    size_t& data_len)
{
    data_len = 0;

    char* pszFindStr = strstr((char*)buff, "\r\nContent-Length:");
    if (pszFindStr) 
    {
        pszFindStr += 15 + 2;
        skip_space(pszFindStr);
        if (sscanf(pszFindStr, "%d", &data_len)) 
        {}

        return e_content_length;
    }

    pszFindStr = strstr((char*)buff, "\r\nTransfer-Encoding:");
    if (pszFindStr) {
        pszFindStr = strstr((char*)pszFindStr, "chunked\r\n");
        if (pszFindStr) {
            return e_content_chunk;
        }
    }
    else
    {
        return e_content_close;
    }
    return e_content_close;
}

bool http::handle_close_response(char* recv_buff, const data_callback& handler)
{
    int recv_ret = 0;
    while (true)
    {
        recv_ret = socket_.recv(recv_buff, HTTPDOWN_RECV_BUFF, 0, 0);
        if (recv_ret <= 0) 
        {
            break;
        }

        // 直接调用，不需要记录长度
        handler(recv_buff, recv_ret);
    }

    return true;
}

bool http::handle_chunk_response(char* recv_buff, const data_callback& handler)
{
    trunk_reader reader(socket_);

    reader.read(handler);

    return !reader.is_error();
}

bool http::handle_content_response(char* recv_buff, const data_callback& handler, size_t content_len)
{
    int recv_ret = 0;

    DWORD saved_length = 0;
    while (true)
    {
        recv_ret = socket_.recv(recv_buff, HTTPDOWN_RECV_BUFF, 0, 0);
        if (recv_ret <= 0) 
        {
            break;
        }

        handler(recv_buff, recv_ret);

        saved_length += recv_ret;
        if (saved_length >= content_len) 
        {
            break;
        }
    }

    return (saved_length >= content_len);
}