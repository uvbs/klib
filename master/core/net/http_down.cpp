// HttpDwonload.cpp: implementation of the http_down class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "http_down.h"

#include "addr_resolver.h"

#pragma warning(disable: 4996)

using namespace klib::net;


const char* g_default_user_agent = "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.17 (KHTML, like Gecko) Chrome/24.0.1312.52 Safari/537.17";

BOOL http_down::parse_url(const char* url, char* hostAddr, int& port, char* getPath) 
{
	if (url == NULL || hostAddr == NULL || getPath == NULL) 
    {
		return FALSE;
	}
	
	const char* temp = strstr(url, "http://");
	if (temp == NULL) 
    {
		return FALSE;
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
	
	return TRUE;
}

BOOL http_down::download(const char* http_url, 
    const char* pszSaveFile,
    const request_header_info & info)
{
    FILE* hFile = fopen(pszSaveFile, "wb");    //创建文件
    if (nullptr == hFile) {
        return FALSE;
    }

    data_callback callback = [&](const char* data, size_t len)
    {
        fwrite(data, len, 1, hFile);
    };

    BOOL result = handle_url_content(http_url, info, callback);
    
    fclose(hFile);

    return result;
}

BOOL http_down::get_url_content(const char* http_url, 
    std::string& str_content,
    const request_header_info& info)
{
    data_callback callback = [&](const char* data, size_t len)
    {
        str_content.append(data, len);
    };
    
    BOOL result = handle_url_content(http_url, info, callback);
    if (!result) {
        str_content.clear();
    }
    
    return result;
}

BOOL http_down::handle_url_content(const char* http_url,
    const request_header_info& info,
    const data_callback& handler)
{
    if (nullptr == http_url ) {
        return FALSE;
    }

    char szHostName[MAX_PATH] = {0};
    char szGet[MAX_PATH] = {0};
    int  port = 0;
    if (!parse_url(http_url, szHostName, port, szGet))
    {
        return FALSE;
    }

    DWORD dwRetryTime = 0;
    m_socket.init();
    while (!m_socket.connect(szHostName, port)) 
    {
        ++ dwRetryTime;
        if (dwRetryTime >= 4) 
        {
            return FALSE;
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

    if (!info.refer_.empty()) 
    {
        request_header.append("Referer: ");
        request_header.append(info.refer_);
        request_header.append("\r\n");
    }

    if (!info.cookie_.empty()) 
    {
        request_header.append("Cookie: ");
        request_header.append(info.cookie_);
        request_header.append("\r\n");
    }

    if (!info.user_agent_.empty()) 
    {
        request_header.append("User-Agent: ");
        request_header.append(info.user_agent_);
        request_header.append("\r\n");
    }
    else 
    {
        request_header.append("User-Agent: ");
        request_header.append(g_default_user_agent);
        request_header.append("\r\n");
    }

    request_header.append("\r\n");

    if (!m_socket.send(request_header.c_str(), request_header.size())) 
    {
        return FALSE;
    }

    char recv_buff[HTTPDOWN_RECV_BUFF];
    DWORD dwRecvedBytes = 0;
    int iRecvRet;
    while (TRUE) 
    {		
        iRecvRet = m_socket.recv(recv_buff + dwRecvedBytes, 1, 0, 0);
        if (iRecvRet <= 0) 
        {
            return FALSE;
        }

        ++ dwRecvedBytes;
        if (dwRecvedBytes > HTTPDOWN_RECV_BUFF) 
        {
            return FALSE;
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
        return FALSE;
    }

    int status = 0; 
    float version = 0.0;
    ULONGLONG startPos, endPos, totalLength;
    startPos = endPos = totalLength = 0;
    if (sscanf(recv_buff, "HTTP/%f %d ", &version, &status) != 2) 
    {
        return FALSE;
    }

    if (status != 200 && status != 206 ) 
    {
        return FALSE;
    }

    DWORD dwFileLength = 0;
    char* pszFindStr = strstr(recv_buff, "Content-Length:");
    if (pszFindStr) 
    {
        if (sscanf(pszFindStr, "Content-Length:%d", &dwFileLength)) 
        {
        }
    }

    if (dwFileLength == 0) 
    {
        iRecvRet = m_socket.recv(recv_buff, 4, 0, 0);
        if (iRecvRet != 4) 
        {
            return FALSE;
        }

        recv_buff[4] = '\0';
        sscanf(recv_buff, "%x", &dwFileLength);
    }

    if (dwFileLength == 0) 
    {
        return FALSE;
    }
    
    DWORD dwSavedLength = 0;
    while (TRUE)
    {
        iRecvRet = m_socket.recv(recv_buff, sizeof(recv_buff), 0, 0);
        if (iRecvRet <= 0) 
        {
            break;
        }

        handler(recv_buff, iRecvRet);

        dwSavedLength += iRecvRet;
        if (dwSavedLength >= dwFileLength) 
        {
            break;
        }
    }

    return (dwSavedLength >= dwFileLength);
}