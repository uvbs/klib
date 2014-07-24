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

file_writer::file_writer(const char* file_name)
    : hfile_(nullptr)
{
    //写入到文件中
    hfile_ = fopen(file_name, "a+");
}

file_writer::~file_writer()
{
    if (nullptr != hfile_)
    {
        fclose(hfile_);
    }
}

size_t file_writer::write(const char* buf, size_t buf_len)
{
    //写入到文件中
    if (NULL == hfile_) {
        return 0;
    }

    return fwrite(buf, 1, buf_len, hfile_);
}

//----------------------------------------------------------------------
//
Logger::Logger()
{
    is_inited_ = false;
    log_level_ = LOG_LEVEL_INFO;
    console_ = NULL;
    max_log_size_ = DEFAULT_MAX_LOG_SIZE;
}

Logger::~Logger() 
{
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

bool Logger::is_inited() const
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
    if (sz < DEFAULT_MIN_LOG_SIZE) {
        return;
    }

    max_log_size_ = sz;
}

void Logger::write_log_raw(LOG_LEVEL level, const char* buf, size_t buf_len)
{
    if (level < log_level_) {
        return;
    }

    std::string file_name = this->get_file_name();
    file_writer log_writer(file_name.c_str());

    write_time_header(&log_writer);
    log_writer.write(buf, buf_len);
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
    size_t buf_len = _tcslen(buffer);
    WriteConsole(console_, buffer, buf_len, &dwWrited, NULL);

    // 写文件中
    auto gbk_str = klib::encode::code_convert::unicode_2_gbk(buffer);
    std::string file_name = this->get_file_name();
    
    file_writer log_writer(file_name.c_str());
    write_time_header(&log_writer);
    log_writer.write(gbk_str.c_str(), gbk_str.size());
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
    size_t buf_len = strlen(buffer);
    WriteConsoleA(console_, buffer, buf_len, &dwWrited, NULL);

    // 写入文件中
    std::string file_name = this->get_file_name();
    file_writer log_writer(file_name.c_str());

    write_time_header(&log_writer);
    log_writer.write(buffer, buf_len);
}

std::string Logger::get_file_name()
{
    time_t now_time;
    char timebuf[26];

    time( &now_time );
    tm* newtime = localtime(&now_time );

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

void Logger::write_time_header(log_device_i* device)
{
    time_t long_time;
    char timebuf[26];

    time( &long_time );
    tm* newtime = localtime(&long_time );

    int ret = sprintf(timebuf, 
        "+++%d-%d-%d %d:%d:%d ",
        newtime->tm_year + 1900,
        newtime->tm_mon + 1,
        newtime->tm_mday,
        newtime->tm_hour,
        newtime->tm_min,
        newtime->tm_sec);

    if (ret > 0) {
        device->write(timebuf, ret);
    }
}

//----------------------------------------------------------------------
//
logger_mgr::logger_mgr() : default_logger_(nullptr)
{
    default_logger_ = Logger::instance();
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
