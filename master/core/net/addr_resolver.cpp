
//#include "stdafx.h"
#include "addr_resolver.h"

//如果没有包含 winsock2则需要包含
using namespace klib::net;


addr_resolver::addr_resolver()
{
	nsize_ = 0;
}

addr_resolver::addr_resolver(const char* ip)
{
	resolve(ip);
}

addr_resolver::~addr_resolver()
{

}

BOOL addr_resolver::resolve(const char* ip)
{
	nsize_ = 0;
	UINT32 uAddr = inet_addr(ip);
	if (INADDR_NONE != uAddr)
    {
		nsize_ = 1;
		m_addrArr[0] = uAddr;
		return TRUE;
	}

	HOSTENT* he = gethostbyname((char *)ip);
	if (NULL == he)
	{
		return FALSE;
	}

	if (he->h_addrtype != AF_INET) 
    {
		return FALSE;
	}

	char* ippos = NULL;
	for (int i=0; i<MAX_ADDRARR_NUM; ++i) 
    {
		if (ippos = he->h_addr_list[i]) 
        {
			m_addrArr[i] = *(UINT32*) ippos;
			nsize_ = i + 1;
		}
		else 
        {
			break;
		}
	}

	return nsize_ > 0;
}
