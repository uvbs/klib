#ifndef _rand_val_h_
#define _rand_val_h_

#include <random>
#include "../core/singleton.h"

namespace klib {
namespace util {

class rand_val : public klib::core::singleton<rand_val>
{
public:
    rand_val();
    rand_val(size_t minv, size_t maxv);

    size_t val();
    size_t val(size_t minv, size_t maxv);

    operator size_t();

protected:
    std::random_device rd_;

    size_t min_val_;
    size_t max_val_;
};


}}



#endif // _rand_val_h_