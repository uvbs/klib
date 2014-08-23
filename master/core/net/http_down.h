
#if !defined(_Klib_HttpDown_H)
#define _Klib_HttpDown_H


#include <functional>
#include <cstdio>
#include <string>
#include "tcp_socket.h"



namespace klib {
namespace net {


class http_down  
{
public:
	http_down() {}
	virtual ~http_down() {}

public:
    ///< 下载url文件到指定的路径
    bool download(const char* http_url, const char* pszSaveFile);

    ///< 直接获取url路径的内容
    bool get_url_content(const char* http_url, std::string& str_content);
};


}}



#endif // !defined(_Klib_HttpDown_H)
