//#include "stdafx.h"
#include "logger.h"
#include <tchar.h>
#include <stdlib.h>

#include "../macro.h"
#include "../io/path.h"

namespace klib{
namespace debug{

#define  DEFAULT_MAX_LOG_SIZE  (10*1024*1024)


Logger::Logger()
{
    is_inited_ = false;
    log_level_ = LOG_LEVEL_INFO;
    console_ = NULL;
    hfile_ = NULL;
    max_log_size_ = DEFAULT_MAX_LOG_SIZE;
}

Logger::~Logger() 
{
    if (nullptr != hfile_)
    {
        fclose(hfile_);
    }
    SAFE_CLOSE_HANDLE(console_);

    uninit();
}

Logger* Logger::instance()
{
    static Logger _instance;

    if (!_instance.is_inited())  {
        std::string app_path;
        io::path::get_app_patha(app_path);

        _instance.init(app_path.c_str());
        _instance.set_log_level(LOG_LEVEL_INFO);
        _instance.set_max_log_size(DEFAULT_MAX_LOG_SIZE);
    }

    return &_instance;
}

void Logger::init(const char* log_path)
{
    if (is_inited_) {
        return;
    }

    this->set_log_path(log_path);
    console_ = GetStdHandle(STD_OUTPUT_HANDLE);
    is_inited_ = true;
}

void Logger::uninit()
{
}

bool Logger::is_inited()
{
    return is_inited_;
}

void Logger::set_log_level(LOG_LEVEL level)
{
    log_level_ = level;
}

void Logger::set_log_path(const char* log_path)
{
    log_path_ = log_path;
}

void Logger::set_max_log_size(size_t sz)
{
    if (sz < 10*1024) {
        return;
    }

    max_log_size_ = sz;
}

void Logger::write_log(LOG_LEVEL level, TCHAR* format, ...)
{
    if (level < log_level_) {
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
    WriteConsole(console_, buffer, _tcslen(buffer), &dwWrited, NULL);
}

void Logger::write_log_a(LOG_LEVEL level, const char* format, ...)
{
    if (level < log_level_) {
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
    WriteConsoleA(console_, buffer, strlen(buffer), &dwWrited, NULL);
}

}}
