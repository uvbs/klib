/***

Copyright 2006 bsmith@qq.com

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/


/*
# 
# 
# @author		bsmith, bsmith@qq.com
# @date			2005-02-01
# @version		beta 0.0.0001
# @corp
# @describe	part of Lance Misc.
# @copyright
# @remark		time
# 

*/


/**
strftime 
Syntax: 
  #include <time.h>
  size_t strftime( char *str, size_t maxsize, const char *fmt, struct tm *time );

[en]
The function strftime() formats date and time information from time to a format specified by fmt, then stores the result in str (up to maxsize characters). Certain codes may be used in fmt to specify different types of time:

Code Meaning 
%a abbreviated weekday name (e.g. Fri) 
%A full weekday name (e.g. Friday) 
%b abbreviated month name (e.g. Oct) 
%B full month name (e.g. October) 
%c the standard date and time string 
%d day of the month, as a number (1-31) 
%H hour, 24 hour format (0-23) 
%I hour, 12 hour format (1-12) 
%j day of the year, as a number (1-366) 
%m month as a number (1-12). Note: some versions of Microsoft Visual C++ may use values that range from 0-11. 
%M minute as a number (0-59) 
%p locale's equivalent of AM or PM 
%S second as a number (0-59) 
%U week of the year, (0-53), where week 1 has the first Sunday 
%w weekday as a decimal (0-6), where Sunday is 0 
%W week of the year, (0-53), where week 1 has the first Monday 
%x standard date string 
%X standard time string 
%y year in decimal, without the century (0-99) 
%Y year in decimal, with the century 
%Z time zone name 
%% a percent sign 

The strftime() function returns the number of characters put into str, or zero if an error occurs.

[chs]
函数strftime()的操作有些类似于sprintf()：识别以百分号(%)开始的格式命令集合，格式化输出结果放在一个字符串中。格式化命令说明串strDest中各种日期和时间信息的确切表示方法。格式串中的其他字符原样放进串中。格式命令列在下面，它们是区分大小写的。

%a 星期几的简写
%A 星期几的全称
%b 月分的简写
%B 月份的全称
%c 标准的日期的时间串
%C 年份的后两位数字
%d 十进制表示的每月的第几天
%D 月/天/年
%e 在两字符域中，十进制表示的每月的第几天
%F 年-月-日
%g 年份的后两位数字，使用基于周的年
%G 年分，使用基于周的年
%h 简写的月份名
%H 24小时制的小时
%I 12小时制的小时
%j 十进制表示的每年的第几天
%m 十进制表示的月份
%M 十时制表示的分钟数
%n 新行符
%p 本地的AM或PM的等价显示
%r 12小时的时间
%R 显示小时和分钟：hh:mm
%S 十进制的秒数
%t 水平制表符
%T 显示时分秒：hh:mm:ss
%u 每周的第几天，星期一为第一天 （值从0到6，星期一为0）
%U 第年的第几周，把星期日做为第一天（值从0到53）
%V 每年的第几周，使用基于周的年
%w 十进制表示的星期几（值从0到6，星期天为0）
%W 每年的第几周，把星期一做为第一天（值从0到53）
%x 标准的日期串
%X 标准的时间串
%y 不带世纪的十进制年份（值从0到99）
%Y 带世纪部分的十进制年份
%z，%Z 时区名称，如果不能得到时区名称则返回空字符。
%% 百分号
*/



#pragma once


#ifdef LANCE_WIN32
	#include <stdio.h>
	#include <time.h>
	#include <windows.h>
//	#ifndef _DEBUG
		#pragma warning(disable : 4996)
//	#endif
#endif

#ifdef LANCE_LINUX
	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
	#include <sys/time.h> 
#endif

#ifdef LANCE_FREEBSD
	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
	#include <sys/time.h> 
#endif


namespace lance
{

/**
win32/linux system time.
*/
namespace systm
{





#ifdef LANCE_LINUX
typedef struct tm T_TIME;
inline long clock()
{
	return ::clock()/1000;
}
inline long tick()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec*1000+tv.tv_usec/1000;
}
inline void sleep(int ms)
{
	struct timespec req;
	req.tv_nsec = ms%1000;
	req.tv_sec = (ms-req.tv_nsec)/1000;
	req.tv_nsec = req.tv_nsec*1000000;
	struct timespec rem;
	nanosleep(&req, &rem);
}
inline void now(T_TIME & time)
{
	time_t ct = ::time(NULL);
	localtime_r(&ct, &time);
	time.tm_year += 1900;
}
inline void utcnow(T_TIME & time)
{
	time_t ct = ::time(NULL);
	gmtime_r(&ct, &time);
	time.tm_year += 1900;
}
#endif

#ifdef LANCE_FREEBSD
typedef struct tm T_TIME;
inline long clock()
{
	return ::clock()/1000;
}
inline long tick()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec*1000+tv.tv_usec/1000;
}
inline void sleep(int ms)
{
	struct timespec req;
	req.tv_nsec = ms%1000;
	req.tv_sec = (ms-req.tv_nsec)/1000;
	req.tv_nsec = req.tv_nsec*1000000;
	struct timespec rem;
	nanosleep(&req, &rem);
}
inline void now(T_TIME & time)
{
	time_t ct = ::time(NULL);
	localtime_r(&ct, &time);
	time.tm_year += 1900;
}
inline void utcnow(T_TIME & time)
{
	time_t ct = ::time(NULL);
	gmtime_r(&ct, &time);
	time.tm_year += 1900;
}
#endif

#ifdef LANCE_WIN32


typedef struct tm T_TIME;
inline long clock()
{
	return ::clock();
}
inline long tick()
{
	return GetTickCount();
}
inline void sleep(int ms)
{
	Sleep(ms);
}
inline void now(T_TIME & time)
{
	SYSTEMTIME ct;
	GetLocalTime(&ct);
	time.tm_mday = ct.wDay;
	time.tm_wday = ct.wDayOfWeek;
	time.tm_hour = ct.wHour;
	time.tm_min = ct.wMinute;
	time.tm_mon = ct.wMonth;
	time.tm_sec = ct.wSecond;
	time.tm_year = ct.wYear;
}
inline void utcnow(T_TIME & time)
{
	SYSTEMTIME ct;
	GetSystemTime(&ct);
	time.tm_mday = ct.wDay;
	time.tm_wday = ct.wDayOfWeek;
	time.tm_hour = ct.wHour;
	time.tm_min = ct.wMinute;
	time.tm_mon = ct.wMonth;
	time.tm_sec = ct.wSecond;
	time.tm_year = ct.wYear;
}
#endif

inline char * format(char * buff, const T_TIME & time)
{
	sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d",
			time.tm_year, time.tm_mon, time.tm_mday, 
			time.tm_hour, time.tm_min, time.tm_sec);
	return buff;
}
inline char * format(char * buff, int len, const char * fmt, const T_TIME & time)
{
	strftime(buff, len, fmt, &time);
	return buff;
}
inline char * fmttime(char * buff, const T_TIME & time)
{
	sprintf(buff, "%02d:%02d:%02d",
			time.tm_hour, time.tm_min, time.tm_sec);
	return buff;
}
inline char * fmtdate(char * buff, const T_TIME & time)
{
	sprintf(buff, "%04d-%02d-%02d",
			time.tm_year, time.tm_mon, time.tm_mday);
	return buff;
}


}

}
















