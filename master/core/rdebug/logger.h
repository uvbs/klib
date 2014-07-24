#ifndef _klib_logger_h
#define _klib_logger_h

//#ifdef _DEBUG

#include <stdio.h>
#include <time.h>
#include <tchar.h>
#include <windows.h>
#include <string>
#include <functional>


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

typedef std::function<std::string(const std::string&, tm*)> log_device_callback;

class log_device_i
{
public:

public:
    virtual ~log_device_i() {}

    virtual size_t write(const char* buf, size_t buf_len) = 0;
    virtual size_t write(const log_device_callback& callback, const char* buf, size_t buf_len)
    {
        return write(buf, buf_len);
    }
};


class file_device : public log_device_i
{
public:
    file_device(const char* log_path);
    file_device() {}
    virtual ~file_device();

    virtual size_t write(const char* buf, size_t buf_len);
    virtual size_t write(const log_device_callback& callback, const char* buf, size_t buf_len);

public:
    void set_log_path(const std::string& log_path);    
    void write_time_header(FILE* hfile);

protected:
    std::string get_file_name(const log_device_callback& callback);

protected:
    std::string             log_path_;

};

// 日志记录器
class Logger
{
public:
    Logger() ;
    ~Logger() ;

public:
    void set_log_path(const char* log_path);
    void set_log_level(LOG_LEVEL level);
    void set_max_log_size(size_t sz);
    void set_log_device(log_device_i*);

    void write_log_raw(log_device_i*, const log_device_callback&, LOG_LEVEL level, const char* buf, size_t buf_len);
    void write_log_raw(log_device_i*, LOG_LEVEL level, const char* buf, size_t buf_len);
    void write_log(log_device_i*, LOG_LEVEL level, TCHAR* format, ...);
    void write_log_a(log_device_i*, LOG_LEVEL level, const char* format, ...);

protected:
    log_device_i* get_device(log_device_i*);

protected:
    LOG_LEVEL       log_level_;         // 日志的级别
    size_t          max_log_size_;

    HANDLE          console_;           // 控制台
    file_device     default_device_;    // 默认写到文件中
    log_device_i*   log_device_;        // 外部设置的
};

// 简单的日志记录管理
class logger_mgr
{
public:
    logger_mgr();
    static logger_mgr* instance();

    Logger* default_logger();
    bool set_default_logger(Logger* loger);

protected:
    Logger* default_logger_;
};

#define  GetLogger()        \
    logger_mgr::instance()->default_logger()

#define LOG_SET_LEVEL(Level)        \
    GetLogger()->set_log_level(Level);

#define LOG(LEVEL, FORMAT, ...)     \
    GetLogger()->write_log(LEVEL, FORMAT, __VA_ARGS__);

#define LOGA(LEVEL, FORMAT, ...)	\
    GetLogger()->write_log_a(nullptr, LEVEL, FORMAT, __VA_ARGS__);

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


#endif
