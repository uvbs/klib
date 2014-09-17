#ifndef _rand_val_h_
#define _rand_val_h_

#include <random>
#include "../core/singleton.h"

namespace klib {
namespace util {

class rand_val : public klib::core::singleton<rand_val>
{
public:
    size_t val();
    size_t val(size_t minv, size_t maxv);

protected:
    std::random_device rd_;
};


}}



#endif // _rand_val_h_