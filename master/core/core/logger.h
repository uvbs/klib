#pragma once

//#ifdef _DEBUG

#include <stdio.h>
#include <time.h>
#include <tchar.h>
#include <windows.h>


namespace klib{
namespace debug{



//日志等级
enum LOG_LEVEL 
{
	LOG_LEVEL_NONE,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FETAL,
};


class Logger 
{
public:
	static Logger* instance();
	void InitLog();
	void UnInitLog();

	void SetLogLevel(LOG_LEVEL level);
	void WriteLog(LOG_LEVEL level, TCHAR* format, ...);
	void WriteLogA(LOG_LEVEL level, const char* format, ...);

protected:
	Logger() ;

protected:
	LOG_LEVEL m_emloglevel;		//日志的级别

	HANDLE m_hConsole;
	FILE*  m_hFile;
};


#define  GetLogger()        \
    Logger::instance()

#define LOG_INIT			\
    Logger::instance()->InitLog();

#define LOG_UNINIT			\
    Logger::instance()->UnInitLog();

#define LOG_SET_LEVEL(Level)		\
    Logger::instance()->SetLogLevel(Level);

#define LOG(LEVEL, FORMAT, ...)	\
    Logger::instance()->WriteLog(LEVEL, FORMAT, __VA_ARGS__);

#define LOGA(LEVEL, FORMAT, ...)	\
    Logger::instance()->WriteLogA(LEVEL, FORMAT, __VA_ARGS__);

#define LOG_INFO(FORMAT, ...)			\
    LOG(LOG_LEVEL_INFO, FORMAT, __VA_ARGS__);

#define LOG_INFOA(FORMAT, ...)			\
    LOGA(LOG_LEVEL_INFO, FORMAT, __VA_ARGS__);

#define LOG_WARNING(FORMAT, ...)			\
    LOG(LOG_LEVEL_WARNING, FORMAT, __VA_ARGS__);

#define LOG_ERROR(FORMAT, ...)			\
    LOG(LOG_LEVEL_ERROR, FORMAT, __VA_ARGS__);

#define LOG_FETAL(FORMAT, ...)			\
    LOG(LOG_LEVEL_FETAL, FORMAT, __VA_ARGS__);


}}
