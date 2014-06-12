#ifndef _klib_datetime_h
#define _klib_datetime_h

/*
类名：tdatetime
描述：日期时间类
作用：1. 从一个给定的日期时间字符串中解析出日期时间信息 
      2. 提供一些常用的日期时间的校验算法
备注：
       该类支持的日期时间格式如下：
    5（五秒）
    4:5(四分五秒)
    5:3:6(五时三分六秒)（注：不是五小时，而是凌晨五时，绝对时间）
    2-28（2月28日）
    2-28 5:3:6（2月28日）
    2008-2-28（2008年2月28日）
    2008-2-28 17:3:6（2008年2月28日17时3分6秒）

        还支持站位符方式：
    -2-  仅设定月份为2月，其余日期采用当前值
    2008-- 仅设定年
    :23: 仅设定分
    -- :: 全部省略，采用当前日期时间作为默认值

    如果不能解析到指定的部分，则采用默认值（默认值为当前日期）
历史：
        2008-11-21     尹曙光（kevdmx@sina.com）  创建
        2013-07-03     董坤                       修改
*/

#ifndef _TDATETIME_H_20081121_
#define _TDATETIME_H_20081121_

//在windows下，如果强制使用32位的time_t，请定义以下宏：
//#ifndef _USE_32BIT_TIME_T
//#define _USE_32BIT_TIME_T
//#endif
#include <time.h>
#include <string>
#include <windows.h>

using std::string;

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

namespace klib{
namespace core{


class tdatetime
{
public:
    unsigned short sYear;   //年
    unsigned char sMonth;   //月
    unsigned char sDay;     //日    
    unsigned char sHour;    //时    
    unsigned char sMinute;  //分
    unsigned char sSecond;  //秒

public:
    //构造函数，采用当前的日期时间作为默认值
    tdatetime();

    //构造函数，从time_t类型的变量中取得日期时间
    tdatetime(time_t t);

    //从字符串中解析出日期时间，未解析到的部分，采用当前默认值
    BOOL ParseDateTimeString(const char *szDateTime);

    //重新为当前的日期时间
    BOOL LoadCurrentDateTime();

    //转化为UNIX形式的time_t时间日期类型
    time_t ToUnixDatetime();

    //转换为字符串
    std::string toString();

    //重新设定为有time_t类型变量指定的日期时间值
    void FromUnixDatetime(time_t t);

    //校验当前对象的日期时间数据是否正确
    BOOL Validate();

    //日期比较
    bool operator > (tdatetime& date);
    bool operator >= (tdatetime& date);
    bool operator < (tdatetime& date);
    bool operator <= (tdatetime& date);

    //增加时间
    static tdatetime& AddSeconds(tdatetime& tm, UINT64 uSecondsToAdd);
    //校验一个tdatetime类型变量的日期时间数据是否正确
    static BOOL Validate(tdatetime *obDateTime);
    //检查年份是否是闰年
    static BOOL IsLeapYear(int year);
    //校验给定的年份是否正确
    static BOOL ValidateDate(int year);
    //校验给定的年份和月分是否正确
    static BOOL ValidateDate(int year,int month);
    //取得给定的年份，给定的月份含有的天数
    static int GetDaysOfMonth(int year, int month);
    //取得给定的年、月、日是星期几(0代表星期天)
    static int GetDayOfWeek(int year, int month, int day);
    //校验给定的年月日数据是否正确
    static BOOL ValidateDate(int year, int month, int day);
    //检验给定的小时数据，是否正确
    static BOOL ValidateTime(int hour);
    //校验给定的小时分钟数据是否正确
    static BOOL ValidateTime(int hour,int minute);
    //校验给定的时间数据是否正确
    static BOOL ValidateTime(int hour, int minute, int second);
    //校验给定的日期时间数据是否正确
    static BOOL ValidateDateTime(int year, int month, int day,
        int hour, int minute, int second);

private:
    //token类型定义
    typedef enum TokenType
    {
        TT_Null = 0,
        TT_Number =1,
        TT_Minus = 2,
        TT_Colon = 4,
        TT_Blank = 8

    };

    //日期时间类型定义
    typedef enum TimePart
    {
        TP_Second = 1,
        TP_Minute = 2,
        TP_Hour = 4,
        TP_Day = 8,
        TP_Month = 16,
        TP_Year = 32
    };

private:
    //与日期相减，获取差值
    int getResult(tdatetime& date);

    //将当前对象变量清零
    void ZeroDateTime(void);

    //根据字符取得该字符所属的类型
    tdatetime::TokenType GetTokenType(char c);
    int m_TimeZoneOffset;
};


}}



#endif  //#ifndef _TDATETIME_H_20081121_

#endif
