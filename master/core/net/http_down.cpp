// HttpDwonload.cpp: implementation of the http_down class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "http_down.h"

#include "addr_resolver.h"

#pragma warning(disable: 4996)

using namespace klib::net;


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

BOOL http_down::download(const char* pszHttpUrl, const char* pszSaveFile)
{
	char szHostName[MAX_PATH];
	char szGet[MAX_PATH];
	int  port;
	if (!parse_url(pszHttpUrl, szHostName, port, szGet))
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

	char szRequestHeader[MAX_PATH * 2];
	//格式化请求头
	int len = sprintf(szRequestHeader, 
		"GET %s HTTP/1.1\r\n"
		"Host: %s\r\n"
		"Connection: close\r\n"
		"Referer: %s\r\n"
		"\r\n",
		szGet,
		szHostName,
		pszHttpUrl);

	if (!m_socket.send(szRequestHeader, len)) 
    {
		return FALSE;
	}

	DWORD dwRecvedBytes = 0;
	int iRecvRet;
	while (TRUE) 
    {		
		iRecvRet = m_socket.recv(m_szRecvBuff + dwRecvedBytes, 1, 0, 0);
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
			if (memcmp(&m_szRecvBuff[dwRecvedBytes - 4], "\r\n\r\n", 4) == 0) 
            {
				break;
			}
		}
	}
	m_szRecvBuff[dwRecvedBytes] = '\0';

	// 解析收到的数据
	if (strnicmp(m_szRecvBuff, "HTTP/", 5)) 
    {
		return FALSE;
	}

	int status = 0; 
	float version = 0.0;
	ULONGLONG startPos, endPos, totalLength;
	startPos = endPos = totalLength = 0;
	if (sscanf(m_szRecvBuff, "HTTP/%f %d ", &version, &status) != 2) 
    {
		return FALSE;
	}
	
	if (status != 200 && status != 206 ) 
    {
		return FALSE;
	}

	DWORD dwFileLength = 0;
	char* pszFindStr = strstr(m_szRecvBuff, "Content-Length:");
	if (pszFindStr) 
    {
		if (sscanf(pszFindStr, "Content-Length:%d", &dwFileLength)) 
        {
		}
	}

	if (dwFileLength == 0) 
    {
		iRecvRet = m_socket.recv(m_szRecvBuff, 4, 0, 0);
		if (iRecvRet != 4) 
        {
			return FALSE;
		}

		m_szRecvBuff[4] = '\0';
		sscanf(m_szRecvBuff, "%x", &dwFileLength);
	}

	if (dwFileLength == 0) 
    {
		return FALSE;
	}

	FILE* hFile = fopen(pszSaveFile, "wb");    //创建文件
	if (hFile == NULL) 
    {
		return FALSE;
	}

	DWORD dwSavedLength = 0;
	while (TRUE)
	{
		iRecvRet = m_socket.recv(m_szRecvBuff, sizeof(m_szRecvBuff), 0, 0);
		if (iRecvRet <= 0) 
        {
			break;
		}

		fwrite(m_szRecvBuff, iRecvRet, 1, hFile);
		dwSavedLength += iRecvRet;
		if (dwSavedLength >= dwFileLength) 
        {
			break;
		}
		Sleep(1);
	}

	fclose(hFile);
	return (dwSavedLength >= dwFileLength);
}

BOOL http_down::get_url_content(const char* pszHttpUrl, std::string& str_content)
{
    char szHostName[MAX_PATH];
    char szGet[MAX_PATH];
    int  port;
    if (!parse_url(pszHttpUrl, szHostName, port, szGet))
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

    char szRequestHeader[MAX_PATH * 2];
    //格式化请求头
    int len = sprintf(szRequestHeader, 
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "Referer: %s\r\n"
        "\r\n",
        szGet,
        szHostName,
        pszHttpUrl);

    if (!m_socket.send(szRequestHeader, len)) 
    {
        return FALSE;
    }

    DWORD dwRecvedBytes = 0;
    int iRecvRet;
    while (TRUE) 
    {		
        iRecvRet = m_socket.recv(m_szRecvBuff + dwRecvedBytes, 1, 0, 0);
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
            if (memcmp(&m_szRecvBuff[dwRecvedBytes - 4], "\r\n\r\n", 4) == 0) 
            {
                break;
            }
        }
    }
    m_szRecvBuff[dwRecvedBytes] = '\0';

    // 解析收到的数据
    if (strnicmp(m_szRecvBuff, "HTTP/", 5)) 
    {
        return FALSE;
    }

    int status = 0; 
    float version = 0.0;
    ULONGLONG startPos, endPos, totalLength;
    startPos = endPos = totalLength = 0;
    if (sscanf(m_szRecvBuff, "HTTP/%f %d ", &version, &status) != 2) 
    {
        return FALSE;
    }

    if (status != 200 && status != 206 ) 
    {
        return FALSE;
    }

    DWORD dwFileLength = 0;
    char* pszFindStr = strstr(m_szRecvBuff, "Content-Length:");
    if (pszFindStr) 
    {
        if (sscanf(pszFindStr, "Content-Length:%d", &dwFileLength)) 
        {
        }
    }

    if (dwFileLength == 0) 
    {
        iRecvRet = m_socket.recv(m_szRecvBuff, 4, 0, 0);
        if (iRecvRet != 4) 
        {
            return FALSE;
        }

        m_szRecvBuff[4] = '\0';
        sscanf(m_szRecvBuff, "%x", &dwFileLength);
    }

    if (dwFileLength == 0) 
    {
        return FALSE;
    }
    
    DWORD dwSavedLength = 0;
    while (TRUE)
    {
        iRecvRet = m_socket.recv(m_szRecvBuff, sizeof(m_szRecvBuff), 0, 0);
        if (iRecvRet <= 0) 
        {
            break;
        }

        str_content.append(m_szRecvBuff, iRecvRet);
        dwSavedLength += iRecvRet;
        if (dwSavedLength >= dwFileLength) 
        {
            break;
        }
        Sleep(1);
    }

    return (dwSavedLength >= dwFileLength);
}