
#include "calendar.h"

namespace klib {
namespace core {



DWORD LunarInfo[] = 
{
    0x04bd8,0x04ae0,0x0a570,0x054d5,0x0d260,0x0d950,0x16554,0x056a0,0x09ad0,0x055d2,
    0x04ae0,0x0a5b6,0x0a4d0,0x0d250,0x1d255,0x0b540,0x0d6a0,0x0ada2,0x095b0,0x14977,
    0x04970,0x0a4b0,0x0b4b5,0x06a50,0x06d40,0x1ab54,0x02b60,0x09570,0x052f2,0x04970,
    0x06566,0x0d4a0,0x0ea50,0x06e95,0x05ad0,0x02b60,0x186e3,0x092e0,0x1c8d7,0x0c950,
    0x0d4a0,0x1d8a6,0x0b550,0x056a0,0x1a5b4,0x025d0,0x092d0,0x0d2b2,0x0a950,0x0b557,
    0x06ca0,0x0b550,0x15355,0x04da0,0x0a5d0,0x14573,0x052d0,0x0a9a8,0x0e950,0x06aa0,
    0x0aea6,0x0ab50,0x04b60,0x0aae4,0x0a570,0x05260,0x0f263,0x0d950,0x05b57,0x056a0,
    0x096d0,0x04dd5,0x04ad0,0x0a4d0,0x0d4d4,0x0d250,0x0d558,0x0b540,0x0b5a0,0x195a6,
    0x095b0,0x049b0,0x0a974,0x0a4b0,0x0b27a,0x06a50,0x06d40,0x0af46,0x0ab60,0x09570,
    0x04af5,0x04970,0x064b0,0x074a3,0x0ea50,0x06b58,0x055c0,0x0ab60,0x096d5,0x092e0,
    0x0c960,0x0d954,0x0d4a0,0x0da50,0x07552,0x056a0,0x0abb7,0x025d0,0x092d0,0x0cab5,
    0x0a950,0x0b4a0,0x0baa4,0x0ad50,0x055d9,0x04ba0,0x0a5b0,0x15176,0x052b0,0x0a930,
    0x07954,0x06aa0,0x0ad50,0x05b52,0x04b60,0x0a6e6,0x0a4e0,0x0d260,0x0ea65,0x0d530,
    0x05aa0,0x076a3,0x096d0,0x04bd7,0x04ad0,0x0a4d0,0x1d0b6,0x0d250,0x0d520,0x0dd45,
    0x0b5a0,0x056d0,0x055b2,0x049b0,0x0a577,0x0a4b0,0x0aa50,0x1b255,0x06d20,0x0ada0
};


BOOL clendar::LeapYear(int year)
{
	return ((year%4==0 && year%100!=0) || year%400==0);
}

int clendar::GetMonthDays(int year, int month)
{
	switch(month)
	{
	case 1:	case 3:	case 5:	case 7:	case 8:	case 10:case 12: return 31;
	case 4:	case 6:	case 9:	case 11: return 30;
	case 2:
		if(LeapYear(year))
			return 29;
		else
			return 28;
	}
	return 0;
}


LONG clendar::DaysFrom1900(custom_date date)
{
	LONG days;
	days = 365 * date.year_ + (date.year_ - 1) / 4 - (date.year_ - 1) / 100 + (date.year_ - 1) / 400-
		(365*1900+(1900-1)/4-(1900-1)/100+(1900-1)/400);
	for(int i=0;i<date.month_-1;i++)
	{
		days += GetMonthDays(date.year_,i+1);
	}
	days += date.day_;
	return days;
}

int clendar::YearDays(int year)
{
	UINT i, sum = 348; 
	for(i=0x8000; i>0x8; i>>=1)
	{
		sum += (LunarInfo[year-1900] & i) ? 1 : 0;
	}
	return(sum + LeapDays(year));
}

int clendar::LeapDays(int year)
{
	if(LeapMonth(year))
	{
		return (LunarInfo[year-1900] & 0x10000) ? 30: 29;
	}
	else
	{
		return 0;
	}
}

int clendar::LeapMonth(UINT year)
{
	return LunarInfo[year-1900] & 0xf;
}

custom_date clendar::GetLunar(custom_date date)
{
	LONG offset;
	int i, leap=0, temp=0;
	custom_date retdate;
	offset=DaysFrom1900(date)-30;
	for(i=1900; i<2050 && offset>0; i++)
	{
		temp = YearDays(i);
		offset -= temp;
	}
	if(offset<0)
	{
		offset += temp;
		i--;
	}
	retdate.year_ = i;
	leap = LeapMonth(i);
	bool isLeap = false;
	for(i=1; i<13 && offset>0; i++)
	{
		if(leap>0 && i==(leap+1) && isLeap==false)
		{
			--i;
			isLeap = true;
			temp = LeapDays(retdate.year_);
		}
		else
		{
			temp = MonthDays(retdate.year_, i);
		}
		if(isLeap==true && i==(leap+1)) 
		{
			isLeap = false;
		}
		offset -= temp;
	}
	if(offset==0 && leap>0 && i==leap+1)
	{
		if(isLeap)
		{ 
			isLeap = false;
		}
		else
		{
			isLeap = true;
			--i;
		}
	}
	if(offset<=0)
	{
		offset += temp;
		--i;
	}
	retdate.month_ = i;
	retdate.day_ = offset ;
	retdate.is_leap_month_ = isLeap;
	return retdate;
}

int clendar::MonthDays(UINT year, UINT month)
{
	return (LunarInfo[year-1900] & (0x10000>>month))? 30: 29;
}

TCHAR* clendar::GetLunarString(custom_date date)
{
	TCHAR szNongli[30], szNongliDay[10], szShuXiang[10];
	const TCHAR *cTianGan[] = {_T("甲"), _T("乙"), _T("丙"), _T("丁"), _T("戊"), _T("己"), _T("庚"), _T("辛"), _T("壬"), _T("癸")}; //天干
	const TCHAR *cDiZhi[]    = {_T("子"), _T("丑"), _T("寅"), _T("卯"), _T("辰"), _T("巳"), _T("午"), _T("未"), _T("申"), _T("酉"), _T("戌"), _T("亥")}; //地支
	const TCHAR *cShuXiang[] = {_T("鼠"), _T("牛"), _T("虎"), _T("兔"), _T("龙"), _T("蛇"), _T("马"), _T("羊"), _T("猴"), _T("鸡"), _T("狗"), _T("猪")}; //生肖
	const TCHAR *cDayName[] = {_T( "*"), _T("初一"), _T("初二"), _T("初三"), _T("初四"), _T("初五"),
			 _T("初六"), _T("初七"), _T("初八"), _T("初九"), _T("初十"), _T("十一"), _T("十二"), _T("十三"), _T("十四"), _T("十五"),
			 _T("十六"), _T("十七"), _T("十八"), _T("十九"), _T("二十"), _T("廿一"), _T("廿二"), _T("廿三"), _T("廿四"), _T("廿五"),       
			 _T("廿六"), _T("廿七"), _T("廿八"), _T("廿九"), _T("三十")}; //农历日期
	const TCHAR *cMonName[] = {_T("*"), _T("正"), _T("二"), _T("三"), _T("四"), _T("五"), _T("六"), _T("七"), _T("八"), _T("九"), _T("十"), _T("十一"), _T("腊")};

	//生成农历天干、地支、属相
	int nShuXiang = ((date.year_ - 4) % 60) % 12;
	if ( nShuXiang < 0 || nShuXiang >= sizeof(cShuXiang)/sizeof(cShuXiang[0]) )
	{
	   return NULL ;
	}
	wsprintf(szShuXiang, _T("%s"), cShuXiang[nShuXiang]);
	int nTianGan = ((date.year_ - 4) % 60) % 10;
	if ( nTianGan < 0 || nTianGan >= sizeof(cTianGan)/sizeof(cTianGan[0]) )
	{
	   return NULL;
	}
	int nDiZhi = ((date.year_ - 4) % 60) % 12;
	if ( nDiZhi < 0 || nDiZhi >= sizeof(cDiZhi)/sizeof(cDiZhi[0]) )
	{
	   return NULL;
	}

	wsprintf(szNongli, _T("%s(%s%s)年"), szShuXiang, cTianGan[nTianGan], cDiZhi[nDiZhi]);

	//生成农历月、日
	if( date.month_ < 0 || date.month_ >= sizeof(cMonName)/sizeof(cMonName[0]) )
	{
		return NULL;
	}

	if(date.is_leap_month_)
	{
		wsprintf(szNongliDay, _T("闰%s"), cMonName[date.month_]);
	}
	else
	{
		wcscpy(szNongliDay, cMonName[date.month_]);
	}

	_tcscat(szNongliDay, _T("月"));
	if( date.day_ < 0 || date.day_ >= sizeof(cDayName)/sizeof(cDayName[0]) )
	{
		return NULL;
	}

	_tcscat(szNongliDay, cDayName[date.day_]);
	_tcscat(szNongli, szNongliDay);
	_stprintf(m_slunar, szNongli);
	return m_slunar;
}

tstring clendar::GetWeekString(int week)
{
	switch(week)
	{
	case 1:
		return _T("星期一");
	case 2:
		return _T("星期二");
	case 3:
		return _T("星期三");
	case 4:
		return _T("星期四");
	case 5:
		return _T("星期五");
	case 6:
		return _T("星期六");
	case 7:
		return _T("星期日");
	}

    _ASSERT(FALSE);
    return _T("");
}


}}
