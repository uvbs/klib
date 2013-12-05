#pragma once

#pragma warning(disable: 4996)

#include <stdio.h>
#include <time.h>
#include <locale>
#include <windows.h>

//#define RELEASE_DEBUG

namespace klib {
namespace debug {


#if defined(_DEBUG) || defined(RELEASE_DEBUG)


/**
 * @brief 日志打印类
 * @usage  
 *
    MyPrtLog("%s", "info");
 */
class CPrtLogFunc
{
public:
  const char * m_file;
  int m_line;

  CPrtLogFunc(const char * file,int line)
  {
    m_file = file;
    m_line = line;
  }

  void operator ()(const char * format,...)
  {
    char newformat[512] = {0};
    char msgbuffer[4*1024] = {0};
    //sprintf_s(newformat, "%s line %d,", m_file, m_line);
    //strcat_s(newformat, format);
    strcpy(newformat, format);
    va_list arg_ptr;
    va_start(arg_ptr, format);
    _vsnprintf_s(msgbuffer, sizeof(msgbuffer), newformat, arg_ptr);
    va_end(arg_ptr);

    //写入到文件中
    tm newtime;
    __time64_t long_time;
    char timebuf[26];

    _time64( &long_time );
    _localtime64_s( &newtime, &long_time );

    //得到日期的字符串
    char logfile[30] = {0};
    sprintf_s(logfile,
              sizeof(logfile),
              "%d-%d-%d_log.txt",
              newtime.tm_year + 1900,
              newtime.tm_mon + 1,
              newtime.tm_mday);

    FILE* fp = NULL;
    fopen_s(&fp, logfile, "a");
    if(fp == NULL) {
	  return;
	}

    char logMsg[4*1024 + 30] = {0};
    sprintf_s(timebuf, sizeof(timebuf),
              "%d-%d-%d %d:%d:%d ",
              newtime.tm_year + 1900,
              newtime.tm_mon + 1,
              newtime.tm_mday,
              newtime.tm_hour,
              newtime.tm_min,
              newtime.tm_sec);
    sprintf_s(logMsg, sizeof(logMsg), "%s: %s \r\n", timebuf, msgbuffer);
    fprintf_s(fp, "%s", logMsg);
    fclose(fp);

    DWORD dwWritten = 0; 
    ::WriteFile(::GetStdHandle(STD_OUTPUT_HANDLE), logMsg, ::strlen(logMsg), &dwWritten, NULL); 
  }
  
  void operator()(TCHAR * format, ...) 
  {
      static BOOL bSetLocal = FALSE;
      TCHAR newformat[512] = {0};
      TCHAR msgbuffer[4*1024] = {0};
      //sprintf_s(newformat, "%s line %d,", m_file, m_line);
      //strcat_s(newformat, format);
      if (!bSetLocal) 
      {
          bSetLocal = TRUE;
          setlocale(LC_ALL, "chs");
      }

      _tcscpy(newformat, format);
      va_list arg_ptr;
      va_start(arg_ptr, format);
      _vstprintf_s(msgbuffer, _countof(msgbuffer), newformat, arg_ptr);
      va_end(arg_ptr);

      //写入到文件中
      tm newtime;
      __time64_t long_time;
      TCHAR timebuf[26];

      _time64( &long_time );
      _localtime64_s( &newtime, &long_time );

      //得到日期的字符串
      TCHAR logfile[30] = {0};
      _stprintf_s(logfile,
          _countof(logfile),
          _T("%d-%d-%d_log.txt"),
          newtime.tm_year + 1900,
          newtime.tm_mon + 1,
          newtime.tm_mday);

      FILE* fp = NULL;
      _tfopen_s(&fp, logfile, _T("a"));
      if(fp == NULL) {
          return;
      }

      TCHAR logMsg[4*1024 + 30] = {0};
      _stprintf_s(timebuf, _countof(timebuf),
          _T("%d-%d-%d %d:%d:%d "),
          newtime.tm_year + 1900,
          newtime.tm_mon + 1,
          newtime.tm_mday,
          newtime.tm_hour,
          newtime.tm_min,
          newtime.tm_sec);
      _stprintf_s(logMsg, _countof(logMsg), _T("%s: %s \r\n"), timebuf, msgbuffer);
      _ftprintf(fp, _T("%s"), logMsg);
      fclose(fp);

      DWORD dwWritten = 0; 
      ::WriteFile(::GetStdHandle(STD_OUTPUT_HANDLE), logMsg, _tcslen(logMsg), &dwWritten, NULL); 
  }
};

#define MyPrtLog CPrtLogFunc(__FILE__,__LINE__)




#else

#define MyPrtLog 

#endif

//----------------------------------------------------------------------
// 调试窗口
class CDebugConsole
{
public:
    static CDebugConsole* GetInstance()
    {
        static CDebugConsole* _instance = new CDebugConsole;
        return _instance;
    }

    ~CDebugConsole(void)
    {
        fclose(stderr);
        fclose(stdout);
        fclose(stdin);
        FreeConsole();
    }

private:
    CDebugConsole(void)
    {
        AllocConsole();   
        SetConsoleTitle(_T("Debug Console"));
        freopen("conin$",  "r+t", stdin);   
        freopen("conout$", "w+t", stdout);   
        freopen("conout$", "w+t", stderr);   
    }
    CDebugConsole(const CDebugConsole&);
    CDebugConsole& operator=(const CDebugConsole&);
};


}}
