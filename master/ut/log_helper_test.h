
#include <rdebug/log_helper.h >

using namespace klib::debug;

TEST(log_helper, 1)
{
    bin_buf buf("world", 5);
    str_buf buf_st(" jlksjdfljdsf", 10);

    log_helper() << 1;

    LOG_IF(1)(1);
    LOG_IF(1) << "hello" << buf << buf_st;

    Logger logger;
    logger.init("d:\\");
    LOG_IF_D(&logger, 1)(std::string("hello "))(buf)(buf_st);


}