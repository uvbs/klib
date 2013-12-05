
#ifndef _Klib_Calendar_H
#define _Klib_Calendar_H

#include "..\..\istddef.h"
#include <windows.h>
#include <tchar.h>


namespace klib {
namespace core {

//@todo 获取一天是星期几，及第几周, 借助 TDateTime类实现

struct custom_date
{
    int     year_;		        //
    int     month_;			    //
    int     day_;			    //
    BOOL    is_leap_month_;		//
};

class clendar
{
public:
    ///< 判断是否是润年
	BOOL LeapYear(int year);

    ///< 获得一个月的天数
	int GetMonthDays(int year, int month);

    ///< 获取从1900年的天数
	LONG DaysFrom1900(custom_date Date);

    ///< 获得一年的天数
	int YearDays(int year);

    ///< 润年的天数
	int LeapDays(int year);

    ///< 
	int LeapMonth(UINT year);

    ///< 获得对应的农历日期
	custom_date GetLunar(custom_date date);

    ///< 获得一个月的天数
	int MonthDays(UINT year, UINT month);

    ///< 获得农历信息
	TCHAR* GetLunarString(custom_date date);

    ///< 获得星期的表示
	tstring GetWeekString(int week);
	
protected:
	TCHAR  m_slunar[100];//鐢ㄤ簬瀛樺偍鍐滃巻淇℃伅
};




}}



#endif
