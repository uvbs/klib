#ifndef _klib_factory_h
#define _klib_factory_h

#include "../istddef.h"
#include "../core/singleton.h"
#include <functional>

namespace klib{
namespace pattern{


using namespace ::klib::core;

template<class product_t>
class factory : public singleton<factory<product_t>>
{
    DECLARE_SINGLETON_CLASS(factory)
public:

    typedef  std::function<product_t* ()>  creator_t;

    bool add(const tstring &class_name, creator_t ceator_imp)
    {
        creators_[class_name] = ceator_imp;
        return true;
    }

    product_t* make(const tstring &class_name)
    {
        creators_map::iterator p = creators_.find(class_name);
        if (p != creators_.end() && p->second)
        {
            return (p->second)();
        }

        return nullptr;
    }

protected:
    struct string_nonecase_sence_cmp : public std::binary_function<tstring, tstring, bool>
    {
        bool operator()(const tstring &left, const tstring &right) const 
        {
            return ::_tcsicmp(left.c_str(), right.c_str()) < 0;
        }
    };


    typedef std::map<tstring, creator_t, string_nonecase_sence_cmp> creators_map;
    creators_map    creators_;
};
















}}



#endif
