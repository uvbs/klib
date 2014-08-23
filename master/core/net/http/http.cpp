// HttpDwonload.cpp: implementation of the http class.
//
//////////////////////////////////////////////////////////////////////

#include "http.h"
#include "../addr_resolver.h"

#pragma warning(disable: 4996)

using namespace klib::net;


#define default_user_agent "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.17 (KHTML, like Gecko) Chrome/24.0.1312.52 Safari/537.17"

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
    stats_method_ = method;
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

    data_callback callback = [&](const char* data, size_t len)
    {
        fwrite(data, len, 1, hFile);
    };

    bool result = handle_requst(http_url, callback);

    fclose(hFile);

    return result;
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
    request_header.append("GET ");
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
    if (stats_method_ == e_stats_pos) 
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

    DWORD dwFileLength = 0;
    char* pszFindStr = strstr(recv_buff, "\r\nContent-Length:");
    if (pszFindStr) 
    {
        pszFindStr += 15 + 2;
        skip_space(pszFindStr);
        if (sscanf(pszFindStr, "%d", &dwFileLength)) 
        {}
    }

    if (0 == dwFileLength) 
    {
        return handle_chunk_response(recv_buff, handler);
    }
    else 
    {
        return handle_content_response(recv_buff, handler, dwFileLength);
    }
}

bool http::handle_chunk_response(char* recv_buff, const data_callback& handler)
{
    int   recv_ret = 0;
    int   buff_data_size = 0;

    size_t data_len   = 0;
    char*  chunk_buf  = nullptr;
    size_t chunk_size = 0;
    size_t extra_size = 0;

    size_t handled_length = 0;

    do 
    {
        recv_ret = socket_.recv(&recv_buff[extra_size], 10, 0, 0);
        if (recv_ret < 3) { // 必须要大于3个: 5\r\n
            return false;
        }
        buff_data_size = recv_ret;

        chunk_parser_result parse_ret = e_parse_continue;
        while (e_parse_continue == parse_ret)
        {
            parse_ret = parser_chunk_info(recv_buff, 
            buff_data_size, 
            data_len, 
            chunk_buf, 
            chunk_size,
            extra_size);

            if (e_parse_error == parse_ret) {
                return false;
            }

            if (e_parse_continue == parse_ret) {
                recv_ret = socket_.recv(&recv_buff[buff_data_size], 10, 0, 0);
                if (recv_ret < 0) {
                    return false;
                }
                buff_data_size += recv_ret;
            }
        }
        if (0 == data_len) {
            break;
        }

        if (data_len <= chunk_size || extra_size > 0) {
            handler(chunk_buf, data_len);
        }
        else {
            handler(chunk_buf, chunk_size - extra_size);
        }
        handled_length = chunk_size - extra_size;
        memmove(recv_buff, recv_buff + (recv_ret - extra_size), extra_size);

        while (handled_length < data_len) 
        {
            size_t left = data_len - handled_length;
            recv_ret = socket_.recv(&recv_buff[extra_size], 
                left >= HTTPDOWN_RECV_BUFF ? (HTTPDOWN_RECV_BUFF - 1) : left,
                0, 
                0);
            if (recv_ret <= 0) 
            {
                break;
            }

            handler(recv_buff, recv_ret);
            handled_length += recv_ret;
            extra_size = 0;
        }

    } while (0 != data_len);

    return (0 == data_len);
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

// <len><\r\n><data...>[extra]
http::chunk_parser_result http::parser_chunk_info(char* recv_buff, 
    size_t buf_len,
    size_t& data_size,
    char*&  chunk_buff,
    size_t& chunk_left,
    size_t& extra_length
    )
{
    int file_len = 0;
    int scan_ret = sscanf(recv_buff, "%x", &file_len);
    data_size = file_len;
    
    recv_buff[buf_len] = 0;
    char* crlf = strstr(recv_buff, "\r\n");
    if (nullptr == crlf || crlf == recv_buff) {
        return e_parse_error;
    }

    char* crlf_end = crlf + 2;
    chunk_left = buf_len - (crlf_end - recv_buff);

    chunk_buff = crlf_end;
    if (data_size >= chunk_left) {
        extra_length = 0;
    }
    else {
        extra_length = buf_len - ((chunk_buff + data_size) - recv_buff);
        if (extra_length >= 2) {
            extra_length -= 2; // \r\n
            return e_parse_ok;
        }
        else {
            return e_parse_continue;
        }
    }

    if (chunk_left > 0 && scan_ret > 0) {
        return e_parse_ok;
    }
    return e_parse_error;
}
