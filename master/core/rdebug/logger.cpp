//#include "stdafx.h"
#include "logger.h"
#include <tchar.h>
#include <stdlib.h>

namespace klib{
namespace debug{


Logger::Logger()
{
	m_emloglevel = LOG_LEVEL_INFO;
	m_hConsole = NULL;
	m_hFile = NULL;
}

Logger* Logger::instance()
{
	static Logger _instance;
    _instance.InitLog();

	return &_instance;
}

void Logger::InitLog()
{
    if (NULL != m_hConsole) 
    {
        return;
    }
	m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
}

void Logger::UnInitLog()
{

}

void Logger::SetLogLevel(LOG_LEVEL level)
{
	m_emloglevel = level;
}

void Logger::WriteLog(LOG_LEVEL level, TCHAR* format, ...)
{
	if (level < m_emloglevel) {
		return;
	}

	TCHAR newformat[500] = {0};
	TCHAR buffer[4*1024];
	_tcscat_s(newformat, format);
	_tcscat_s(newformat, _T("\r\n"));

	va_list arg_ptr;
	va_start(arg_ptr, format);
	_vstprintf_s(buffer,  _countof(buffer), newformat,  arg_ptr);
	va_end(arg_ptr); 

	DWORD dwWrited;
	WriteConsole(m_hConsole, buffer, _tcslen(buffer), &dwWrited, NULL);
}

void Logger::WriteLogA(LOG_LEVEL level, const char* format, ...)
{
	if (level < m_emloglevel) 
    {
		return;
	}

	char newformat[500] = {0};
    char buffer[4*1024] = {0};
	strcat(newformat, format);
	strcat(newformat, "\r\n");

	va_list arg_ptr;
	va_start(arg_ptr, format);
	vsprintf_s(buffer,  _countof(buffer), newformat,  arg_ptr);
	va_end(arg_ptr); 

	DWORD dwWrited;
	WriteConsoleA(m_hConsole, buffer, strlen(buffer), &dwWrited, NULL);
}

}}
