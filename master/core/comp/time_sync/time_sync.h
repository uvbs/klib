#pragma once

#include <windows.h>
#include <vector>
#include <time.h>

//+ time_sync_impl
class time_sync_impl
{
public:
    virtual bool set_retry_times(UINT retryTimes);
    virtual int  get_internet_time(std::string& strTime) ; //获得标准时间
    virtual int  get_nettm_throught_elapse(std::string& strTime) ;
    virtual int  get_nettm_throught_elapse(tm& theStdTime) ;
    virtual bool sync_time();
    virtual bool is_time_ok();     //判断获取网络时间是否成功
    virtual bool get_local_time(std::string& strTime); //获取本机时间

public:
    time_sync_impl(void);
    ~time_sync_impl(void);

private:
    void itime_2_stime(SYSTEMTIME &st, const ULONG ulTime);
    void time_2_str(std::string& strTime, SYSTEMTIME &st);

private:
    UINT          retryt_times_; //重试次数
    bool          time_recved_; //表示是否接收到的网络时间

    SYSTEMTIME    inet_sys_time_; //保存从网络上获取的时间
    ULONG         unet_time_;     //从网络上获取的时间值
    DWORD         inet_tick_count_; //当获到时间时的clock计数
};