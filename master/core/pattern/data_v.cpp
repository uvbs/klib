
#include "data_v.h"



std::string data_v::get_stats()
{
    auto itr = this->m_item_lst.begin();
    std::string str;

    for (; itr != this->m_item_lst.end(); ++ itr)
    {
        str.append(itr->name);
        str.append(":");
        str.append(itr->f());
        str.append("\r\n");
    }

    return std::move(str);
}

bool data_v::add_watch(const std::string& name, const int& v)
{
    item t;
    t.f = [&v]()->std::string 
    {
        return std::to_string((long long)v);
    };

    t.name = name;
    m_item_lst.push_back(t);

    return true;
}

bool data_v::add_watch(const std::string& name, const size_t& v)
{
    item t;
    t.f = [&v]()->std::string 
    {
        return std::to_string((long long)v);
    };

    t.name = name;
    m_item_lst.push_back(t);

    return true;
}


bool data_v::add_watch(const std::string& name, std::function<std::string()> f)
{
    item t;
    t.f = f;

    t.name = name;
    m_item_lst.push_back(t);

    return true;
}

bool data_v::add_watch(const std::string& name, std::function<size_t()> f)
{
    item t;
    t.f = [f]()->std::string 
    {
        return std::to_string(f());
    };

    t.name = name;
    m_item_lst.push_back(t);

    return true;
}

bool data_v::add_watch(const std::string& name, std::function<int()> f)
{
    item t;
    t.f = [f]()->std::string 
    {
        return std::to_string((size_t) f());
    };

    t.name = name;
    m_item_lst.push_back(t);

    return true;
}