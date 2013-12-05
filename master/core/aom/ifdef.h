#pragma once

#include "istddef.h"

// 定义接口的
// {3ADB422F-65EF-4359-81E6-F26E23E760E8}
static const GUID IID_IUtil = 
{ 0x3adb422f, 0x65ef, 0x4359, { 0x81, 0xe6, 0xf2, 0x6e, 0x23, 0xe7, 0x60, 0xe8 } };

// {8CC8506C-6AB9-4406-8DD9-9CAD995BBD11}
static const GUID IID_INetwork = 
{ 0x8cc8506c, 0x6ab9, 0x4406, { 0x8d, 0xd9, 0x9c, 0xad, 0x99, 0x5b, 0xbd, 0x11 } };

// {21297091-D456-4F1D-879F-729205D492F1}
static const GUID IID_ICalendar = 
{ 0x21297091, 0xd456, 0x4f1d, { 0x87, 0x9f, 0x72, 0x92, 0x5, 0xd4, 0x92, 0xf1 } };



struct IUtil 
{
    //获得应用程序的路径，有反斜杠
    virtual std::string GetAppPath() = 0;
    //获得应用程序的目录，无反斜杠
    virtual std::string GetAppDir() = 0;

    //获得应用程序名称,例如 ox.exe
    virtual std::string GetAppName() = 0;

    //获各文件扩展名
    virtual bool GetFileExt(std::string& strExt, const char* filepath) = 0;
    virtual bool GetFileExtW(std::wstring& strExt, const wchar_t* filepath) = 0;
    virtual bool GetFileName(std::string& strFileName, const char* filepath) = 0;
    virtual bool GetFileNameW(std::wstring& strFileName, const wchar_t* filepath) = 0;
    virtual bool GetFilePath(std::string& strPath, const char* filepath) = 0;
    virtual bool GetFilePathW(std::wstring& strPath, const wchar_t* filepath) = 0;
    virtual bool GetFileDir(std::string& strDir, const char* filepath) = 0;
    virtual bool GetFileDirW(std::wstring& strDir, const wchar_t* filepath) = 0;

    virtual bool ExpandEnvironment(std::string& deststr, const char* strVariable) = 0;

    /*
    使用ie打开url，注意如果使用默认的浏览器打不开的时候要在注册表查找.htm关联并用其来打开网页
    */
    virtual bool OpenURL(LPCTSTR pszURL) = 0;

    virtual bool HasFullScreenProgram() = 0;
    //开机自动启动
    virtual bool SetAppRunBoot(LPCTSTR pszStartTitle, bool bBoot = true) = 0;

    virtual bool ReduceMemory() = 0; //减少内存使用

    virtual bool CreateDesktopUrl(LPCTSTR pszName, const char* url, DWORD hotKey) = 0; //创建url链接
    virtual bool CreateUrl(LPCTSTR pszPath, const char* url, DWORD hotKey) = 0; //创建url链接
    //----------------------------------------------------------------------
    // Summary:
    //		创建多级目录
    // Parameters:
    //		pszDirPath		-	
    // Returns:
    //		
    //----------------------------------------------------------------------
    virtual BOOL MakeSureDirectoryPathExists(LPCTSTR pszDirPath) = 0;

    virtual bool GetExeNameFromWnd(HWND hWnd, LPTSTR  pszBuf, UINT nSize) = 0; //根据窗口句柄，获取对应用程序路径

    virtual bool RefreshDesktop() = 0; //刷新桌面
    virtual HWND FixOnDesktop(HWND hWndToFix, bool bFix = true) = 0;  //定在桌面

    //获取操作系统信息
    virtual bool GetOsInfo(OSVERSIONINFOEX& osvi) = 0;

    //获取操作系统版本
    virtual bool GetOsVersion(std::string &version) = 0;

    //判断是否是子目录下的
    virtual bool IsChildDir(const char* strMain, const char* strSub, bool bCheckSub) = 0;

    //判断一个字符串是否是网址
    virtual bool IsWebSite(const char *str) = 0;
    virtual bool SetDirectoryAliasName(LPCTSTR pszDir, LPCTSTR pszAliasName) = 0; //设置文件夹的别名
    //将绝对路径转换为相对路径
    virtual bool AbsolutePath2RelativePath(std::string& strPath, const char* strBasePath, const char* target, const char* PathVar="{AppPath}") = 0;

    virtual DWORD GetCPUUse() = 0; //获得cpu的使用率，0~100
    virtual BOOL EnablePrivilege(LPCTSTR lpszPrivilegeName, BOOL bEnable) = 0;
    virtual bool IsSystemDir(LPCTSTR pszPath) = 0;    //是否是系统目录
    virtual BOOL IsEnableUAC() = 0;       //是否启用了UAC
    virtual BOOL IsAdmin() =0;            //是否是admin用户
    virtual BOOL ShutdownSys(BOOL bReboot = FALSE) = 0;       //关闭计算机，及重启计算机
    virtual BOOL IsProcessRun(LPCTSTR pszExeName) = 0; //查看指定的进程是否在运行

    virtual BOOL FilterSqlParam(std::string& str) = 0;
    virtual BOOL FilterQueryParam(std::string& str, const char chEscape = '\\') = 0;

    virtual BOOL IsTextUTF8(const char* str,ULONGLONG length) = 0;      //判断是否是utf8编码的
};



typedef std::map<std::string, std::string> PostParam;

enum emTestNetworkType {
    //----------------------------------------------------------------------
    // 根据域名解析判断
    //----------------------------------------------------------------------
    emTestHostName,      

    //----------------------------------------------------------------------
    // 根据ping指定的主机
    //----------------------------------------------------------------------
    emTestPing,

    //----------------------------------------------------------------------
    // 根据以上两个来判断
    //----------------------------------------------------------------------
    emTestBoth
};

//----------------------------------------------------------------------
// Summary:
//      网络操作相关接口
//----------------------------------------------------------------------
struct INetwork
{
public:
    //----------------------------------------------------------------------
    // Summary:
    //		判断网络是否连接的
    // Parameters:
    //		type		-       检测类型
    // Returns:
    //		连接上了返回true,失败返回false
    //----------------------------------------------------------------------
    virtual bool IsConnectedByTest(emTestNetworkType type) = 0; 

    //----------------------------------------------------------------------
    // Summary:
    //		 判断是否连接上了，直接返回结果
    // Parameters:
    // Returns:
    //		与网络连接的话返回true，没有连接的话返回false
    //----------------------------------------------------------------------
    virtual bool GetIsConnected() = 0;

    //----------------------------------------------------------------------
    // Summary:
    //		获取url的内容
    // Parameters:
    //		url		-	
    //		content		-	
    // Returns:
    //		成功返回true，失败返回false
    //----------------------------------------------------------------------
    virtual bool GetUrlString(const char* url, std::string& strContent) = 0;//获得url的内容

    //----------------------------------------------------------------------
    // Summary:
    //      根据url下载文件
    //----------------------------------------------------------------------
    virtual bool URLDownloadFile(const char* url, const char* destpath) = 0; 
};

//----------------------------------------------------------------------
// Summary:
//      日历模块
//----------------------------------------------------------------------
struct ICalendar
{
public:
    //----------------------------------------------------------------------
    // Summary:
    //      得到星期天 例：0~6
    //----------------------------------------------------------------------
    virtual bool  GetWeeks(int& week,int year, int month, int day) =0;

    //----------------------------------------------------------------------
    // Summary:
    //      指定月份(公历)得到这月有多少天
    //----------------------------------------------------------------------
    virtual bool GetDaysAccordingMonth(int& days,int year,int month) =0;

    //----------------------------------------------------------------------
    // Summary:
    //      得打农历字符串 如：龙年 润四月初一
    //----------------------------------------------------------------------
    virtual bool  GetLunarStr(std::string& szLunar,int year,int month,int day)=0;

    //----------------------------------------------------------------------
    // Summary:
    //      得到天干地支  如： 辛卯年 癸巳月 乙丑日
    //----------------------------------------------------------------------
    virtual bool  GetGanZhiStr(std::string& szGanZhi,int year,int month,int day)=0;

    //宜忌 szShould返回宜  szAvoid返回忌，需要数据库支持
    //virtual bool GetShouldAvoid(std::string& szShould,std::string& szAvoid,int year,int month,int day) = 0;

    //----------------------------------------------------------------------
    // Summary:
    //      节日 如： 世界红十字日 国际母亲节
    //----------------------------------------------------------------------
    virtual bool GetFestival(std::string& szFestival,int year,int month,int day) = 0;
};
