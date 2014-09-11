#ifndef _klib_rand_str_h_
#define _klib_rand_str_h_

#include <string>

namespace klib {
namespace util {

class rand_str
{
public:
    rand_str() ;

    bool set_str_tbl(const char* buf, size_t len);
    std::string get_rand_str(size_t len);

protected:
    const char* str_tbl_;
    size_t      tbl_len_;

};



}}















#endif 