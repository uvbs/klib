#ifndef _klib_logger_h
#define _klib_logger_h

//#ifdef _DEBUG

#include <stdio.h>
#include <time.h>
#include <tchar.h>
#include <windows.h>
#include <string>


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


class log_writer
{
public:
    void init(const std::string& path);
    std::string get_file_name();
    size_t write(const char* buf, size_t buf_len);
    void write_time_header();

protected:
    std::string file_path_;
};

// 日志记录器
class Logger 
{
public:
    Logger() ;
    ~Logger() ;

public:
    static Logger* instance();
    void init(const char* log_path);
    void uninit();
    bool is_inited();

    void set_log_path(const char* log_path);
    void set_log_level(LOG_LEVEL level);
    void set_max_log_size(size_t sz);

    void write_log_raw(LOG_LEVEL level, const char* buf, size_t buf_len);
    void write_log(LOG_LEVEL level, TCHAR* format, ...);
    void write_log_a(LOG_LEVEL level, const char* format, ...);

protected:
    bool            is_inited_;
    LOG_LEVEL       log_level_;     //日志的级别

    HANDLE          console_;
    FILE*           hfile_;
    std::string     log_path_;
    size_t          max_log_size_;
    log_writer      log_writer_;
};


#define  GetLogger()        \
    Logger::instance()

#define LOG_SET_LEVEL(Level)        \
    Logger::instance()->set_log_level(Level);

#define LOG(LEVEL, FORMAT, ...)     \
    Logger::instance()->write_log(LEVEL, FORMAT, __VA_ARGS__);

#define LOGA(LEVEL, FORMAT, ...)	\
    Logger::instance()->write_log_a(LEVEL, FORMAT, __VA_ARGS__);

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
