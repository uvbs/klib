//#include "stdafx.h"
#include "logger.h"
#include <tchar.h>
#include <stdlib.h>

#include "../macro.h"
#include "../io/path.h"
#include "../core/scope_guard.h"
#include "../core/code_convert.h"

using namespace klib::core;


namespace klib{
namespace debug{

#define  DEFAULT_MIN_LOG_SIZE  (10*1024)
#define  DEFAULT_MAX_LOG_SIZE  (10*1024*1024)

//----------------------------------------------------------------------
//

file_device::file_device(const char* log_path)
    : log_path_(log_path)
{
}

file_device::~file_device()
{
}

size_t file_device::write(const char* buf, size_t buf_len)
{
    std::string str_file_name = this->get_file_name(nullptr);

    //写入到文件中
    FILE* hfile = fopen(str_file_name.c_str(), "a+");
    if (NULL == hfile) {
        return 0;
    }
    defer (
        fclose(hfile);
    );

    write_time_header(hfile);

    write_console(buf, buf_len);

    return fwrite(buf, 1, buf_len, hfile);
}

size_t file_device::write(const log_device_callback& callback, 
    const char* buf, 
    size_t buf_len)
{
    std::string str_file_name = this->get_file_name(callback);

    //写入到文件中
    FILE* hfile = fopen(str_file_name.c_str(), "a+");
    if (NULL == hfile) {
        return 0;
    }
    defer (
        fclose(hfile);
    );

    write_time_header(hfile);

    write_console(buf, buf_len);
    return fwrite(buf, 1, buf_len, hfile);
}

void file_device::set_log_path(const std::string& log_path)
{
#ifdef _UNICODE
    std::wstring wstr = encode::code_convert::gbk_2_unicode(log_path);
    io::path::create_directorys(wstr);
#else
    io::path::create_directorys(log_path);
#endif

    log_path_ = log_path;
}

void file_device::write_time_header(FILE* hfile)
{
    time_t long_time;
    char timebuf[26];

    time( &long_time );
    tm* newtime = localtime(&long_time );

    int ret = sprintf(timebuf, 
        "%d-%d-%d %d:%d:%d ",
        newtime->tm_year + 1900,
        newtime->tm_mon + 1,
        newtime->tm_mday,
        newtime->tm_hour,
        newtime->tm_min,
        newtime->tm_sec);

    if (ret > 0) 
    {
        write_console(timebuf, ret);
        fwrite(timebuf, 1, ret, hfile);
    }
}

std::string file_device::get_file_name(const log_device_callback& callback)
{
    time_t now_time;

    time( &now_time );
    tm* newtime = localtime(&now_time );

    if (callback)
    {
        return callback(log_path_, newtime);
    }

    //得到日期的字符串
    char log_file_name[1024] = {0};

    sprintf(log_file_name,
        "%s%d-%d-%d_log.txt",
        log_path_.c_str(),
        newtime->tm_year + 1900,
        newtime->tm_mon + 1,
        newtime->tm_mday);

    return  log_file_name;
}

void file_device::write_console(const char* buff, size_t buf_len)
{
    DWORD dwWritten;
    ::WriteFile(::GetStdHandle(STD_OUTPUT_HANDLE), 
        buff, 
        buf_len, 
        &dwWritten, 
        NULL); 
}


//----------------------------------------------------------------------
//
Logger::Logger()
{
    log_level_ = LOG_LEVEL_INFO;
    max_log_size_ = DEFAULT_MAX_LOG_SIZE;
    console_ = GetStdHandle(STD_OUTPUT_HANDLE);
    log_device_ = nullptr;
}

Logger::~Logger() 
{
    SAFE_CLOSE_HANDLE(console_);

}

void Logger::set_log_level(LOG_LEVEL level)
{
    log_level_ = level;
}

void Logger::set_log_path(const char* log_path)
{
    default_device_.set_log_path(log_path);
}

void Logger::set_max_log_size(size_t sz)
{
    if (sz < DEFAULT_MIN_LOG_SIZE) {
        return;
    }

    max_log_size_ = sz;
}

void Logger::set_log_device(log_device_i* dev)
{
    KLIB_ASSERT(dev);

    log_device_ = dev;
}

void Logger::write_log_raw(log_device_i* device, const log_device_callback& callback, LOG_LEVEL level, const char* buf, size_t buf_len)
{
    if (level < log_level_) {
        return;
    }

    // 写入设备中
    log_device_i* writer_device = get_device(device);
    writer_device->write(callback, buf, buf_len);
}

void Logger::write_log_raw(log_device_i* device, LOG_LEVEL level, const char* buf, size_t buf_len)
{
    if (level < log_level_) {
        return;
    }

    // 写入设备中
    log_device_i* writer_device = get_device(device);
    writer_device->write(buf, buf_len);
}

void Logger::write_log(log_device_i* device, LOG_LEVEL level, TCHAR* format, ...)
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
    size_t buf_len = _tcslen(buffer);
    WriteConsole(console_, buffer, buf_len, &dwWrited, NULL);

#ifdef _UNICODE
    std::string gbk_str = klib::encode::code_convert::unicode_2_gbk(buffer);
#else
    std::string gbk_str = buffer;
#endif
    
    // 写入设备中
    log_device_i* writer_device = get_device(device);
    writer_device->write(gbk_str.c_str(), gbk_str.size());
}

void Logger::write_log_a(log_device_i* device, LOG_LEVEL level, const char* format, ...)
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
    size_t buf_len = strlen(buffer);
    WriteConsoleA(console_, buffer, buf_len, &dwWrited, NULL);

    // 写入设备中
    log_device_i* writer_device = get_device(device);
    writer_device->write(buffer, buf_len);
}

log_device_i* Logger::get_device(log_device_i* dev)
{
    if (nullptr != dev)
    {
        return dev;
    }

    if (nullptr != log_device_)
    {
        return log_device_;
    }

    return & default_device_;
}

//----------------------------------------------------------------------
//
logger_mgr::logger_mgr() : default_logger_(nullptr)
{
    default_logger_ = new Logger;

    std::string app_path;
    io::path::get_app_patha(app_path);

    default_logger_->set_log_path(app_path.c_str());
    default_logger_->set_log_level(LOG_LEVEL_INFO);
    default_logger_->set_max_log_size(DEFAULT_MAX_LOG_SIZE);
}

logger_mgr* logger_mgr::instance()
{
    static  logger_mgr _instance;
    return & _instance;
}

Logger* logger_mgr::default_logger()
{
    return default_logger_;
}

bool logger_mgr::set_default_logger(Logger* loger)
{
    default_logger_ = loger;
    return true;
}


}}
