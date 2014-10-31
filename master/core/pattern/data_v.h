#ifndef _klib_data_v_h_
#define _klib_data_v_h_


#include <vector>
#include <list>
#include <string>
#include <functional>


class data_v
{
public:
    struct item
    {
        std::string name;
        std::function<std::string()> f;
    };

    std::string get_stats();
    
    template <typename T>
    bool add_watch(const std::string& name, const T& v);
    bool add_watch(const std::string& name, const int& v);
    bool add_watch(const std::string& name, const size_t& v);
    bool add_watch(const std::string& name, std::function<std::string()>);
    bool add_watch(const std::string& name, std::function<size_t()>);
    bool add_watch(const std::string& name, std::function<int()>);

protected:
    std::vector<item> m_item_lst;
};


template <typename T>
bool data_v::add_watch(const std::string& name, const T& v)
{
    item t;
    t.f = [&v]()->std::string 
    {
        return std::to_string(v);
    };

    t.name = name;
    m_item_lst.push_back(t);

    return true;
}


#define  ADD_INT_WATCH(dt, v)\
    dt.add_watch(#v, v);


#endif // _klib_data_v_h_