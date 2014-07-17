

#if !defined(_Klib_Md5_H)
#define _Klib_Md5_H

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "../inttype.h"


//@todo 添加使用说明

namespace klib {
namespace encode{

/* // 因为类型冲突 
#ifdef __alpha
typedef unsigned int uint32;
#else
typedef unsigned long uint32;
#endif
*/

#include <TCHAR.H>

#ifndef byte
typedef unsigned char byte;
#endif

struct MD5Context {
	uint32 buf[4];
	uint32 bits[2];
	unsigned char in[64];
};

class CMD5  
{
public:
	static void MD5Update(struct MD5Context *ctx, unsigned char *buf, unsigned len);
	static void MD5Final(unsigned char digest[16], struct MD5Context *ctx);
	static void MD5Transform(uint32 buf[4], uint32 in[16]);
	static void MD5Init(struct MD5Context *ctx);
	static char* MD5String(const char* pstr, char* pstrresult);
	static char* MD5String(const wchar_t* pstr, char* pstrresult);
	static byte* MD5Data(byte* pData, uint32 length, byte* pResult, uint32 rLength);
private:
	CMD5();
	virtual ~CMD5();
};


}} // namespace 

#endif // !defined()
