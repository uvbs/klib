#ifndef _klib_uuid_h_
#define _klib_uuid_h_


#include "../inttype.h"
#include <string>


namespace klib {
namespace core {

// 创建uuid
std::string create_uuid();


// twitter snowflake算法
// 64       63--------------22---------12---------0
// 符号位   |     41位时间   |10位机器码|12位自增码|
extern uint64_t get_time();

class unique_id_t
{
public:
    unique_id_t();
    ~unique_id_t();

    void set_epoch(uint64_t epoch);
    void set_machine(int32_t machine);
    int64_t generate();

private:
    uint64_t epoch_;
    uint64_t time_;
    int32_t machine_;
    int32_t sequence_;
};

}}

#endif // _klib_uuid_h_