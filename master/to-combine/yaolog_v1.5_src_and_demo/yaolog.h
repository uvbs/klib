/*******************************************************************************
YaoLog v1.5

Description: A powerful, easy-use, cross-platform C++ log utility
Updated:     Jan 24, 2013
Author:      Neil.L
E-mail:      Neil.L@qq.com
********************************************************************************/


#ifndef _YAO_LOG_H_
#define _YAO_LOG_H_

///////////////////////////////////////////////////////////////
// platform
///////////////////////////////////////////////////////////////
#ifndef _YAO_LOG_PLATFORM_DEFINED_
    #if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
        #define _YAO_LOG_WIN32_
    #else
        #define _YAO_LOG_POSIX_
    #endif
    #define _YAO_LOG_PLATFORM_DEFINED_
#endif


///////////////////////////////////////////////////////////////
// include
///////////////////////////////////////////////////////////////
#ifdef _YAO_LOG_WIN32_
#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#else
#include <stdarg.h>
#include <pthread.h>
#include <arpa/inet.h>
#endif

#include <string>
#include <map>
#include <vector>
#include <cassert>


///////////////////////////////////////////////////////////////
// Uncomment the block below if you want use dll.
// define YAOLOG_EXPORTS for dll itself.
///////////////////////////////////////////////////////////////
/*
#ifdef _YAO_LOG_WIN32_
    #ifdef YAOLOG_EXPORTS
        #define YAO_LOG_EXPORT_API _declspec(dllexport)
    #else
        #define YAO_LOG_EXPORT_API _declspec(dllimport)
    #endif
#else
    #define YAO_LOG_EXPORT_API
#endif
*/


///////////////////////////////////////////////////////////////
// for protect... 
///////////////////////////////////////////////////////////////
#ifndef YAO_LOG_EXPORT_API
#define YAO_LOG_EXPORT_API
#endif


///////////////////////////////////////////////////////////////
// charset
///////////////////////////////////////////////////////////////
#ifndef _YAO_LOG_CHARSET_DEFINED_
    #if defined(_YAO_LOG_WIN32_) && defined(_UNICODE)
        #define _YAO_LOG_UNICODE_
    #else
        #define _YAO_LOG_ANSI_
    #endif
    #define _YAO_LOG_CHARSET_DEFINED_
#endif


///////////////////////////////////////////////////////////////
// string
///////////////////////////////////////////////////////////////
#ifndef yao_tstring
    #ifdef _YAO_LOG_UNICODE_
        #define yao_tstring   std::wstring
    #else
        #define yao_tstring   std::string
        #ifndef _T
        #define _T(x)     x
        #endif
        typedef char      TCHAR;
    #endif
#endif


///////////////////////////////////////////////////////////////
// Macros for log
// These log marcos will not be compiled if _NO_YAO_LOG_ defined...
///////////////////////////////////////////////////////////////
#ifdef _NO_YAO_LOG_

#ifndef LOGBIN__
    #define LOGBIN__(logID, data_, size_)
#endif
#ifndef LOG__
    #define LOG__(logID, szFormat, ...)
#endif
#ifndef LOGA__
    #define LOGA__(logID, szFormat, ...)
#endif
#ifndef LOGW__
    #define LOGW__(logID, szFormat, ...)
#endif

#else

#ifndef LOGBIN__
    #define LOGBIN__(logID, data_, size_) do {\
        if (YaoUtil::LogFactory::IsTotallyDisabled()) break;\
        YaoUtil::LogFactory::m_tm.Lock();\
        YaoUtil::BaseLog *p = YaoUtil::LogFactory::Get(logID);\
        if ((!p) || (p->IsTextLog())) { YaoUtil::LogFactory::m_tm.Unlock(); break; }\
        p->LogBin(data_, size_);\
        YaoUtil::LogFactory::m_tm.Unlock();\
        } while (0)
#endif
#ifndef LOG__
    #ifdef _YAO_LOG_WIN32_
        #define LOG__(logID, szFormat, ...) do {\
            if (YaoUtil::LogFactory::IsTotallyDisabled()) break;\
            YaoUtil::LogFactory::m_tm.Lock();\
            YaoUtil::BaseLog *p = YaoUtil::LogFactory::Get(logID);\
            if ((!p) || (!p->IsTextLog())) { YaoUtil::LogFactory::m_tm.Unlock(); break; }\
            p->Log(__FILE__, __FUNCTION__, __LINE__, szFormat, __VA_ARGS__);\
            YaoUtil::LogFactory::m_tm.Unlock();\
            } while (0)
    #else
        #define LOG__(logID, szFormat, ...) do {\
            if (YaoUtil::LogFactory::IsTotallyDisabled()) break;\
            YaoUtil::LogFactory::m_tm.Lock();\
            YaoUtil::BaseLog *p = YaoUtil::LogFactory::Get(logID);\
            if ((!p) || (!p->IsTextLog())) { YaoUtil::LogFactory::m_tm.Unlock(); break; }\
            p->Log(__FILE__, __FUNCTION__, __LINE__, szFormat, ##__VA_ARGS__);\
            YaoUtil::LogFactory::m_tm.Unlock();\
            } while (0)
    #endif
#endif
#ifndef LOGA__
    #ifdef _YAO_LOG_WIN32_
        #define LOGA__(logID, szFormat, ...) do {\
            if (YaoUtil::LogFactory::IsTotallyDisabled()) break;\
            YaoUtil::LogFactory::m_tm.Lock();\
            YaoUtil::BaseLog *p = YaoUtil::LogFactory::Get(logID);\
            if ((!p) || (!p->IsTextLog())) { YaoUtil::LogFactory::m_tm.Unlock(); break; }\
            p->LogA(__FILE__, __FUNCTION__, __LINE__, szFormat, __VA_ARGS__);\
            YaoUtil::LogFactory::m_tm.Unlock();\
            } while (0)
    #else
        // linux has no LOGA__...
    #endif
#endif
#ifndef LOGW__
    #ifdef _YAO_LOG_WIN32_
        #define LOGW__(logID, szFormat, ...) do {\
            if (YaoUtil::LogFactory::IsTotallyDisabled()) break;\
            YaoUtil::LogFactory::m_tm.Lock();\
            YaoUtil::BaseLog *p = YaoUtil::LogFactory::Get(logID);\
            if ((!p) || (!p->IsTextLog())) { YaoUtil::LogFactory::m_tm.Unlock(); break; }\
            p->LogW(__FILE__, __FUNCTION__, __LINE__, szFormat, __VA_ARGS__);\
            YaoUtil::LogFactory::m_tm.Unlock();\
            } while (0)
    #else
        // linux has no LOGW__...
    #endif
#endif

#endif


namespace YaoUtil {
///////////////////////////////////////////////////////////////
// TinyMutex
///////////////////////////////////////////////////////////////
class YAO_LOG_EXPORT_API TinyMutex
{
public:
    TinyMutex();
    ~TinyMutex();
    void Lock();
    void Unlock();

private:
#ifdef _YAO_LOG_WIN32_
    CRITICAL_SECTION m_handle;
#else
    pthread_mutex_t m_handle;
#endif

private:
    TinyMutex(const TinyMutex&);
    const TinyMutex& operator=(const TinyMutex&);
};


///////////////////////////////////////////////////////////////
// StrUtil
///////////////////////////////////////////////////////////////
class StrUtil
{
public:
    static void vFormat(yao_tstring& s, const TCHAR *fmt, va_list ap);
    static void vFormatA(std::string& s, const char *fmt, va_list ap);
    static bool CompareNoCase(const std::string& strIn1, const std::string& strIn2);
    static std::string GetSysTimeStr(bool withMillisecond);
    static std::string GetSysTimeStrForFileName(bool isBinary);
    static int SplitString(
        const std::string& strIn,
        const std::string& strDelimiter,
        std::vector<std::string>& ret,
        bool retWithEmpty);
    static size_t IndexOfFirst(const std::string& strIn, const std::string& strMatch);
    static size_t IndexOfLast(const std::string& strIn, const std::string& strMatch);

    //in:  http://www.baidu.com:80/1.html
    //out: www.baidu.com  /1.html  80
    static bool ParseUrl(
        const std::string& strUrl,
        std::string& strServer,
        std::string& strPath,
        int& nPort);

#ifdef _YAO_LOG_WIN32_
    static void vFormatW(std::wstring& s, const wchar_t *fmt, va_list ap);
    static std::string WStrToStr(const std::wstring& strIn);
    static std::wstring StrToWStr(const std::string& strIn);
#endif
};


///////////////////////////////////////////////////////////////
// PathUtil
///////////////////////////////////////////////////////////////
class PathUtil
{
public:
    static std::string ModuleFileDirectory_();

    static std::string GetDirectory_(const std::string& fileFullPath);

    static std::string GetFile_(const std::string& fileFullPath);

    // directory+folder or directory+file
    static std::string PathCombine_(const std::string& path1, const std::string& path2);

    // path_ -> directory or fileFullPath
    static bool PathFileExists_(const std::string& path_);

    static bool IsStrJustFileName_(const std::string& str);

    static void CreateFolder_(const std::string& directory_);
};


///////////////////////////////////////////////////////////////
// IniReader
///////////////////////////////////////////////////////////////
class IniReader
{
public:
    static bool ReadStrA(const std::string& section, const std::string& key_,
        std::string& value_, const std::string& defaultValue,
        const std::string& filePath);

    static int ReadIntA(const std::string& section, const std::string& key_,
        int defaultValue, std::string& filePath);

private:
    static int ReadStr_(const char *section, const char *key_, char *value_,
        int size_, const char *defaultValue, const char *file);
    static int LoadIniFile(const char *file, char *buf, int *file_size);
    static int NewLine(char c);
    static int EndOfStr(char c);
    static int LeftBrace(char c);
    static int RightBrace(char c);
    static int ParseFile(
        const char *section, const char *key_, const char *buf,
        int *sec_s, int *sec_e, int *key_s, int *key_e, int *value_s, int *value_e);
};


///////////////////////////////////////////////////////////////
// MachineID
///////////////////////////////////////////////////////////////
class MachineID
{
public:
    // I use MAC address as a machine's unique identifier.
    // Maybe it is not an "absolute unique identifier" in reality,
    // but it is good enough to meet our requirements here.
    static std::string GetMachineID();

private:
#ifdef _YAO_LOG_WIN32_
    static long GetMACAddressMSW(unsigned char * result);
#else
    static long GetMACAddressLinux(unsigned char * result);
#endif
};


///////////////////////////////////////////////////////////////
// FileUtil
///////////////////////////////////////////////////////////////
#ifdef _YAO_LOG_WIN32_
typedef HANDLE P_FILE;
#else
typedef FILE* P_FILE;
#endif
class FileUtil
{
public:
    static P_FILE Open(const std::string& path_, bool overwrite_);
    static void Write(P_FILE pFile, const char *pBuf, unsigned int bufSize);
    static void Close(P_FILE pFile);
    static bool Valid(P_FILE pFile);
};


///////////////////////////////////////////////////////////////
// Encoding
///////////////////////////////////////////////////////////////
class Encoding
{
public:
    // base64 without newline
    static std::string Base64Encode(const unsigned char *bytes_to_encode, unsigned int in_len);
    static std::string Base64Decode(std::string const& encoded_string);

    // see http://en.wikipedia.org/wiki/Percent-encoding
    static std::string UrlEncode(const std::string& str);
    static std::string UrlDecode(const std::string& str);

private:
    static bool IsBase64(unsigned char c);
    static unsigned char ToHex(unsigned char x);
    static unsigned char FromHex(unsigned char x);
    const static std::string base64_chars;
};


///////////////////////////////////////////////////////////////
// HttpConn
///////////////////////////////////////////////////////////////
class HttpConn
{
public:
    HttpConn(const std::string& host_, int port_);
    ~HttpConn();

    bool connect();
    void close();

    // method -> "GET", "POST" etc...
    // url    -> "/index.html"
    void putrequest(const char *method_, const char *url_);
    void putheader(const std::string& header_, const std::string& value_);
    void putheader(const std::string& header_, int numericvalue);
    void endheaders();
    void send(const unsigned char *buf, int numbytes);
    void rev();

private:
    // Try to work out address from string
    // returns 0 if bad
    struct in_addr host2addr(const char *host_name);

    std::string m_host;
    int m_port;
    int m_sock;
    std::vector<std::string> m_buffer;
};


///////////////////////////////////////////////////////////////
// TinyThread
///////////////////////////////////////////////////////////////
class TinyThread;

struct ThreadStartInfo
{
    void (*pFn)(void *);
    void *pArg;
    int cycleSpanMilliseconds;
    bool *pThreadEnd;
    TinyThread *pThread;
};

class TinyThread
{
public:
    TinyThread();

    void SetSig(int timeout_millisecond);
    void Join(int timeout_millisecond);
    bool Start(
        void (*pFn)(void *),
        void *pArg,
        int cycleSpanMilliseconds,
        bool *pThreadEnd);

private:
#ifdef _YAO_LOG_WIN32_
    HANDLE m_handle;
    unsigned int m_threadID;
    HANDLE m_event;
#else
    pthread_t m_handle;
    pthread_cond_t m_cond;
    pthread_mutex_t m_mutex;
#endif

#ifdef _YAO_LOG_WIN32_
    static unsigned WINAPI Run(void *pArg);
#else
    static void *Run(void *pArg);
#endif
};


///////////////////////////////////////////////////////////////
// Log classes
///////////////////////////////////////////////////////////////
enum LOGOUT_FLAG
{
    LOGOUT_FLAG_FILE               = 1<<0,
    LOGOUT_FLAG_STDOUT             = 1<<1,
    LOGOUT_FLAG_REMOTE             = 1<<2,
    LOGOUT_FLAG_OUTPUTDEBUGSTRING  = 1<<3  // windows only
};

struct LogParams
{
    bool bEnable;
    int nOutFlag;
    bool bWithTime;
    bool bWithMillisecond;
    bool bWithSrcFile;
    bool bWithFunction;
    bool bOverwrite;           // for LOGOUT_FLAG_FILE
    std::string logFileDir;
    std::string logFileName;
    std::string destUrl;

    LogParams()
    {
        Reset();
    }

    void Reset()
    {
        bEnable = false;
        nOutFlag = LOGOUT_FLAG_STDOUT;
        bWithTime = true;
        bWithMillisecond = false;
        bWithSrcFile = false;
        bWithFunction = false;
        bOverwrite = true;
        logFileDir.clear();
        logFileName.clear();
        destUrl.clear();
    }

    bool operator==(const LogParams &lp) const
    {
        return (bEnable == lp.bEnable &&
            nOutFlag == lp.nOutFlag &&
            bWithTime == lp.bWithTime &&
            bWithMillisecond == lp.bWithMillisecond &&
            bWithSrcFile == lp.bWithSrcFile &&
            bWithFunction == lp.bWithFunction &&
            bOverwrite == lp.bOverwrite &&
            logFileDir == lp.logFileDir &&
            logFileName == lp.logFileName &&
            destUrl == lp.destUrl);
    }  
};

class BaseLog;

class YAO_LOG_EXPORT_API LogFactory
{
public:
    static void Init();
    static void Exit();

    // bTextLog: TextLog or BinaryLog
    static bool Create(const std::string& logID, bool bEnable, bool bTextLog = true);

    static BaseLog *Get(const std::string& logID);

    // 1. pass empty string "", not a NULL, to indicate use default path
    //    default logFileDir -> "module file dir\log\"
    //    default logFileName -> "logID_time.log" or "logID_time.bl"
    // 2. destUrl -> server address, for example "http://192.168.1.200/default.aspx"
    // 3. post data content:
    //      logID=v1&isText=v2&machineID=v3&logData=v4
    //      v2 -> 0 or 1
    //      v4 -> log data, have used base64encode and urlencode
    static void SetAttr(
        const std::string& logID,
        bool bEnable,
        int nOutFlag = LOGOUT_FLAG_STDOUT,
        bool bWithTime = true,
        bool bWithMillisecond = false,
        bool bWithSrcFile = false,
        bool bWithFunction = false,
        bool bOverwrite = true,
        const std::string& logFileDir = "",
        const std::string& logFileName = "",
        const std::string& destUrl = "");

    // ini_: config file's name(in module file dir) or config file's full path
    static void SetAttrFromConfigFile(const std::string& logID, const std::string& ini_);

    // call TotallyDisableAllLogs(true) will disable all logs totally, 
    // then all logs are disabled and cannot be enable
    // the only way to cancel it is by calling TotallyDisableAllLogs(false)
    static void TotallyDisableAllLogs(bool disableAll) { m_totallyDisableAllLog = disableAll; }
    static bool IsTotallyDisabled() { return m_totallyDisableAllLog; }

    static TinyMutex m_tm;

private:
    //static void Destroy(const std::string& logID);
    static bool Exists(const std::string& logID);
    static void BkWorker(void *pArg);
    static void PostData(
        const std::string& logID,
        const std::vector<std::string>& logData);

    static std::map<std::string, BaseLog*> m_allLoggers;
    static bool m_threadEnd;
    static TinyThread *m_pTT;
    static bool m_inited;
    static bool m_totallyDisableAllLog;

    LogFactory(const LogFactory&);
    const LogFactory& operator=(const LogFactory&);
};

class YAO_LOG_EXPORT_API BaseLog
{
    friend class LogFactory;

public:
    BaseLog(const std::string& logID, bool bEnable);
    virtual ~BaseLog();

    void SetAttr(
        bool bEnable,
        int nOutFlag,
        bool bWithTime,
        bool bWithMillisecond,
        bool bWithSrcFile,
        bool bWithFunction,
        bool bOverwrite,
        const std::string& logFileDir,
        const std::string& logFileName,
        const std::string& destUrl);

    void SetAttrFromConfigFile(const std::string& ini_);

    const LogParams* GetLogParams() { return &m_params; }

    bool IsTextLog() { return !m_bBinary; }

    virtual void Log(const char *szSrcFile, const char *szFunction,
        int nLine, const TCHAR *szFormat, ...) = 0;
#ifdef _YAO_LOG_WIN32_
    virtual void LogA(char *szSrcFile, char *szFunction,
        int nLine, const char *szFormat, ...) = 0;
    virtual void LogW(char *szSrcFile, char *szFunction,
        int nLine, const wchar_t *szFormat, ...) = 0;
#endif
    virtual void LogBin(void *pData, int nSize) = 0;

protected:
    bool m_bBinary;
    LogParams m_params;
    std::string m_iniFullPath;
    std::string m_logFullPath;
    std::string m_logID;

    void SetLogFilePath();
    bool ParseConfigFile(LogParams& lp);

    // lazy-init: doesn't open the log file until actually start to write...
    P_FILE GetFilePointer();

private:
    BaseLog(const BaseLog&);
    const BaseLog& operator=(const BaseLog&);
};

class YaoLog : public BaseLog
{
public:
    YaoLog(const std::string& logID, bool bEnable);
    virtual ~YaoLog();
    virtual void Log(const char *szSrcFile, const char *szFunction,
        int nLine, const TCHAR *szFormat, ...);
#ifdef _YAO_LOG_WIN32_
    virtual void LogA(char *szSrcFile, char *szFunction,
        int nLine, const char *szFormat, ...);
    virtual void LogW(char *szSrcFile, char *szFunction,
        int nLine, const wchar_t *szFormat, ...);
#endif

private:
    int MakeLogText(
        const std::string& str,
        const std::string& srcFile,
        const std::string& function_,
        int nLine);

    void DoLog(
        const std::string& str,
        const std::string& srcFile,
        const std::string& function_,
        int nLine);

    // unuseful...
    virtual void LogBin(void *pData, int nSize) { assert(0); }
};

class YaoBinLog : public BaseLog
{
public:
    YaoBinLog(const std::string& logID, bool bEnable);
    virtual void LogBin(void *pData, int nSize);

private:
    // unuseful...
    virtual void Log(const char *szSrcFile, const char *szFunction,
        int nLine, const TCHAR *szFormat, ...) { assert(0); }
#ifdef _YAO_LOG_WIN32_
    virtual void LogA(char *szSrcFile, char *szFunction,
        int nLine, const char *szFormat, ...) { assert(0); }
    virtual void LogW(char *szSrcFile, char *szFunction,
        int nLine, const wchar_t *szFormat, ...) { assert(0); }
#endif
};

} // namespace YaoUtil

#endif // _YAO_LOG_H_

//////////////////////////////////////////////////////////
// config file example
//
// [MyLogID]
// Enable = 1
// OutFlag = file|stdout|remote|outputdebugstring
// WithTime = 1
// WithMillisecond = 0
// WithSrcFile = 0
// WithFunction = 0
// Overwrite = 1
// LogFileDir = c:\logfile
// LogFileName = log1.txt
// DestUrl = http://...
//////////////////////////////////////////////////////////