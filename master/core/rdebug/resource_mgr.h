#ifndef _klib_resource_mgr_h
#define _klib_resource_mgr_h

#include <map>


namespace klib {
namespace debug {

template<class Res>
class resource_mgr
{
public:
    struct res_info
    {
        char desc[50];
        size_t add_tm;
    };

    size_t size() const;

    bool add_res(const Res& res, const char* desc);
    bool del_res(const Res& res);

protected:
    std::map<Res, res_info> res_table_;
};

template<class Res>
size_t resource_mgr<Res>::size() const
{
    return res_table_.size();
}

template<class Res>
bool resource_mgr<Res>::add_res(const Res& res, const char* desc)
{
    auto itr = res_table_.find(res);
    if (itr != res_table_.end())
    {
        return false;
    }

    res_info& info = res_table_[res];
    strncpy(info.desc, desc, sizeof(info.desc));

    return true;
}

template<class Res>
bool resource_mgr<Res>::del_res(const Res& res)
{
    auto itr = res_table_.find(res);
    if (itr == res_table_.end())
    {
        return false;
    }

    res_table_.erase(itr);
    return true;
}


}}



#endif // _klib_resource_mgr_h