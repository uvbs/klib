
#if !defined(_Klib_IpAddressResolver_H)
#define _Klib_IpAddressResolver_H


#include <WinSock2.h>
#include <windows.h>


namespace klib {
namespace net {

///< 定义容纳解析的ip个数
#define  MAX_ADDRARR_NUM	5

///< ip地址解析器
class addr_resolver  
{
public:
	addr_resolver();
	addr_resolver(const char* ip);
	virtual ~addr_resolver();

    ///< 解析ip地址
	BOOL resolve(const char* ip);

    ///< 获取IP地址的个数
    inline size_t size() { return nsize_; }

    ///< 获取索的地址值
    inline UINT at(UINT uIndex);

protected:
	size_t  nsize_;
	UINT32  m_addrArr[MAX_ADDRARR_NUM];
};

///< 获取指定索引的ip地址
UINT addr_resolver::at(size_t uIndex)
{
    if (uIndex >=0 && uIndex < nsize_) 
    {
        return m_addrArr[uIndex];
    }

    return 0;
}


}}




#endif // !defined(_Klib_IpAddressResolver_H)
