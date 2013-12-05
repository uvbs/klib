
#ifndef _Klib_INTTYPE_H_
#define _Klib_INTTYPE_H_


typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef signed char int_least8_t;
typedef short int_least16_t;
typedef int int_least32_t;

typedef unsigned char uint_least8_t;
typedef unsigned short uint_least16_t;
typedef unsigned int uint_least32_t;

typedef char int_fast8_t;
typedef int int_fast16_t;
typedef int int_fast32_t;

typedef unsigned char uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;

typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

typedef __int64 int_least64_t;
typedef unsigned __int64 uint_least64_t;

typedef __int64 int_fast64_t;
typedef unsigned __int64 uint_fast64_t;

typedef __int64 intmax_t;
typedef unsigned __int64 uintmax_t;


#ifndef WIN32
#include <sys/types.h>
#endif

typedef char				int8;
typedef unsigned char		uint8;
typedef short				int16;
typedef unsigned short		uint16;
typedef int					int32;
typedef unsigned int		uint32;
#ifdef WIN32
typedef __int64				int64;
typedef unsigned __int64	uint64;
#else
#include <sys/types.h>
typedef int64_t				int64;
typedef u_int64_t			uint64;
#endif


#endif //_ADVERTISEMENT_INTTYPE_H_