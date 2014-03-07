
#if !defined(_Klib_HttpDown_H)
#define _Klib_HttpDown_H


#include <cstdio>
#include <string>
#include "tcp_socket.h"



namespace klib {
namespace net {


///< 定义下载接收缓冲区大小
#define  HTTPDOWN_RECV_BUFF			(2*1024)


//----------------------------------------------------------------------
// 
// User-Agent:Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.17 (KHTML, like Gecko) Chrome/24.0.1312.52 Safari/537.17
//----------------------------------------------------------------------

class http_down  
{
public:
	http_down() {}
	virtual ~http_down() {}

public:
    ///< 解析URL地址
	BOOL parse_url(const char* url, char* hostAddr, int& port, char* getPath) ;

    ///< 下载url文件到指定的路径
	BOOL download(const char* pszHttpUrl, const char* pszSaveFile);

    ///< 直接获取url路径的内容
	BOOL get_url_content(const char* pszHttpUrl, std::string& str_content);
    	
protected:
	tcp_socket m_socket;                        ///< TCP套接字对象
	char m_szRecvBuff[HTTPDOWN_RECV_BUFF];      ///< 接收缓冲区  
};


}}



#endif // !defined(_Klib_HttpDown_H)
