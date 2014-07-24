

#include "tracer.h"


namespace klib{
namespace debug{

tracer::tracer()
    : switch_(true)
{
    device_.set_log_path("E:\\");
}

tracer::~tracer()
{

}

tracer* tracer::instance()
{
    static tracer _instance;
    return & _instance;
}


Logger* tracer::get_logger()
{
    return &logger_;
}


thread_id_type tracer::get_thread_id()
{
#ifdef _WIN32
    return GetCurrentThreadId();
#else

#endif

}


}}