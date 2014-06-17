
#include <rdebug/log_helper.h >

using namespace klib::debug;

TEST(log_helper, 1)
{
    bin_buf buf_b("\r\thello thisis afd skjlsdfdfsdfsdf", 20);
    str_buf buf_st(" jlksjdfljdsf", 10);

    log_helper() << 1;

    LOG_IF(1)(1);
    LOG_IF(1) << "hello" << buf_b << buf_st;

    Logger logger;
    logger.init("d:\\");
    LOG_IF_D(&logger, 1)(std::string("hello "))(buf_b)(buf_st);


}