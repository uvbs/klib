#pragma once

#include <string>

// {9509EAA7-A395-4881-8971-01A756FEA483}
static const GUID IID_ITimeSync = 
{ 0x9509eaa7, 0xa395, 0x4881, { 0x89, 0x71, 0x1, 0xa7, 0x56, 0xfe, 0xa4, 0x83 } };



struct ITimeSync
{
public:
    virtual void SetRetryTimes(UINT retryTimes) = 0;
    virtual int  GetInternetTime(std::string& strTime) = 0; //获得标准时间
    virtual bool  GetNetTmTroughElapse(std::string& strTime) = 0;
    virtual bool  GetNetTmTroughElapse(tm& theStdTime) = 0;
    virtual bool SyncTime() = 0;
    virtual bool IsGetTimeOK() = 0; //返回是否获取到了标准时间
    virtual bool GetLocalTime(std::string& strTime) = 0; //获得本机时间
};