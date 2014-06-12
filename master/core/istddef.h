#ifndef _klib_istddef_h
#define _klib_istddef_h

#include <string>
#include <map>

namespace klib
{


#ifdef _UNICODE
typedef     std::wstring    tstring ;
#else
typedef     std::string     tstring ;
#endif


#ifndef __in
	#define  __in		// 输出参数
	#define __out		// 输出参数
	#define __inout		// 输入同时输出
#endif





}

#endif
