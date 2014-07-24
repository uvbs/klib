
#include <rdebug/tracer.h>


TEST(tracert_test, 1)
{
    auto id_ = GetCurrentThreadId();    

    const std::string str("tracer 1");

    TRACE_PATH("E:\\TEST\\tracer\\");
    TRACE_STR(str.c_str())(str);
    TRACE_ID(tracer_2)("dsfdsfdsf");
    TRACE_ID(tracer_3)(43434);
    TRACE_ID(tracer 4)(852144655);


    TRACE_THREAD()("thread id test");
}