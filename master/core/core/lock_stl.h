#pragma once


#include "../kthread/auto_lock.h"

#include <functional>
#include <map>
#include <hash_map>

namespace klib {
namespace stl {

using namespace klib::kthread;

//----------------------------------------------------------------------
// 有锁链接基类
template<class T, class StlContainer>
class CLockListBase
{
public:
    typedef CLockListBase<T, StlContainer>  self_type;
    typedef StlContainer  ContainerType;

public:
    size_t size() const
    {
        auto_lock lock(m_auto_cs);

        return m_container.size();
    }

    bool find_item(const T& t)
    {
        auto_lock lock(m_auto_cs);

        auto itr = m_container.begin();
        for (; itr != m_container.end(); ++itr)
        {
            if (*itr == t)
            {
                return true;
            }
        }

        return false;
    }

    bool push_item(const T& t)
    {
        auto_lock lock(m_auto_cs);

        m_container.push_back(t);
        return true;
    }
    
    bool pop_item(T& t)
    {
        auto_lock lock(m_auto_cs);

        if (m_container.empty()) 
        {
            return false;
        }

        t = m_container.front();
        m_container.pop_front();
        return true;
    }

    bool remove_item(T& t)
    {
        auto_lock lock(m_auto_cs);

        m_container.remove(t);
        return true;
    }

    void for_each(std::function<bool(T& t)> f)
    {
        auto_lock lock(m_auto_cs);

        auto itr = m_container.begin();
        for (; itr != m_container.end(); ++ itr)
        {
            if (!f(*itr))  break;
        }
    }
    
    StlContainer& get_container()
    {
        return m_container;
    }

    auto_cs& get_lock()
    {
        return m_auto_cs;;
    }

protected:
    mutable auto_cs m_auto_cs;;        ///< 锁
    StlContainer m_container;               ///< 容器
};

//----------------------------------------------------------------------
// 有锁链接
template<class T>
class CLockList : public CLockListBase<T, std::list<T>>
{
public:
    typedef CLockList<T>  self_type;
    typedef std::list<T>   ContainerType;

};

//----------------------------------------------------------------------
// 有锁vector基类
template<class T, class StlContainer>
class CLockVectorBase
{
public:
    size_t size() const
    {
        auto_lock lock(m_auto_cs);

        return m_container.size();
    }

    bool find_item(const T& t)
    {
        auto_lock lock(m_auto_cs);

        auto itr = m_container.begin();
        for (; itr != m_container.end(); ++itr)
        {
            if (*itr == t)
            {
                return true;
            }
        }

        return false;
    }

    bool remove_item(const T& t)
    {
        auto_lock lock(m_auto_cs);

        auto itr = m_container.begin();
        for (; itr != m_container.end(); ++itr)
        {
            if (*itr == t)
            {
                m_container.erase(itr);
                return true;
            }
        }

        return false;
    }

    bool push_item(const T& t)
    {
        auto_lock lock(m_auto_cs);

        m_container.push_back(t);
        return true;
    }
    
    bool pop_item(T& t)
    {
        auto_lock lock(m_auto_cs);

        if (m_container.empty()) 
        {
            return false;
        }

        t = m_container.front();
        m_container.erase(m_container.begin());
        return true;
    }

    void for_each(std::function<void(T& t)> f)
    {
        auto_lock lock(m_auto_cs);

        auto itr = m_container.begin();
        for (; itr != m_container.end(); ++ itr)
        {
            f(*itr);
        }
    }

    StlContainer& get_container()
    {
        return m_container;
    }


    auto_cs& get_lock()
    {
        return m_auto_cs;;
    }

protected:
    mutable auto_cs m_auto_cs;;        ///< 临界区
    StlContainer m_container;       ///< 容器
};

//----------------------------------------------------------------------
// 有锁vector
template<class T>
class CLockVector : public CLockVectorBase<T, std::vector<T>>
{
public:

};


//----------------------------------------------------------------------
// 有锁stack
template<class T>
class CLockStack 
{
public:
    bool push_item(const T& t)
    {
        auto_lock lock(m_auto_cs);

        m_container.push_back(t);
        return true;
    }

    bool pop_item(T& t)
    {
        auto_lock lock(m_auto_cs);

        if (m_container.empty()) 
        {
            return false;
        }

        t = m_container.back();
        m_container.pop_back();
        return true;
    }

    size_t size()
    {
        return m_container.size();
    }

    auto_cs& get_lock()
    {
        return m_auto_cs;;
    }

protected:
    auto_cs     m_auto_cs;;
    std::vector<T>  m_container;
};


//----------------------------------------------------------------------
// 锁对基类
template<class Key, class Val, class StlContainer>
class CLockPairBase
{
public:
    ///<  获取大小
    size_t size() const
    {
        return m_container.size();
    }

    ///< 获取键值对中的值
    bool get_item(const Key& k, Val& v)
    {
        auto_lock lock(m_auto_cs);

        auto itr = m_container.find(k);
        if (itr != m_container.end()) 
        {
            v = itr->second;
            return true;
        }
        return false;
    }

    ///< 查找是否存在该值
    bool find_item(const Key& k)
    {
        auto_lock lock(m_auto_cs);

        auto itr = m_container.find(k);
        if (itr == m_container.end())
        {
            return false;
        }
        return true;
    }

    ///< 添加一项
    bool add_item(const Key& k, const Val& v)
    {
        auto_lock lock(m_auto_cs);

        auto itr = m_container.find(k);
        if (itr != m_container.end()) 
        {
            return false;
        }

        m_container.insert(std::make_pair(k, v));
        return true;
    }

    ///< 更新item
    bool update_item(const Key& k, const Val& v, BOOL bAdd)
    {
        auto_lock lock(m_auto_cs);

        auto itr = m_container.find(k);
        if (itr == m_container.end()) 
        {
            if (!bAdd) 
            {
                return false;
            }
            
            m_container.insert(std::make_pair(k, v));
        }
        else
        {
            itr->second = v;
        }

        return true;
    }

    ///< 删除一项
    bool remove_item(const Key& k)
    {
        auto_lock lock(m_auto_cs);

        m_container.erase(k);

        return true;
    }

    ///< 根据条件删除项,matchfun 可以对v进行释放操作
    void remove_if(std::function<bool(const Key& k, Val& v)> matchfun)
    {
        auto_lock lock(m_auto_cs);

        auto itr = m_container.begin();
        for (; itr != m_container.end(); )
        {
            if (matchfun(itr->first, itr->second))
            {
                itr = m_container.erase(itr);
            }
            else
            {
                ++ itr;
            }
        }
    }

    ///< 检测某一项
    void check_item(const Key& k, std::function<void (Val& v)> checkfun)
    {
        auto_lock lock(m_auto_cs);

        auto itr = m_container.find(k);
        if (itr != m_container.end()) 
        {
            checkfun(itr->second);
        }

        return;
    }

    ///< 释放其中的一项,会调用delete
    bool delete_item(const Key& k)
    {
        auto_lock lock(m_auto_cs);

        auto itr = m_container.find(k);
        if (itr != m_container.end()) 
        {
            delete itr->second;
        }
        m_container.erase(itr);
        return true;
    }

    ///< 提供遍历的接口
    void for_each(std::function<void(const Key& k, const Val& v)> f)
    {
        auto_lock lock(m_auto_cs);

        auto itr = m_container.begin();
        for (; itr != m_container.end(); ++ itr)
        {
            f(itr->first, itr->second);
        }
    }

protected:
    StlContainer m_container;
    mutable auto_cs m_auto_cs;;
};

//----------------------------------------------------------------------
// LockMap
template<class Key, class Val>
class CLockMap : public CLockPairBase<Key, Val, std::map<Key, Val>>
{
public:    
};

//----------------------------------------------------------------------
// LockHashMap
template<class Key, class Val>
class CLockHashMap : public CLockPairBase<Key, Val, stdext::hash_map<Key, Val>>
{
public:
};

//----------------------------------------------------------------------
// 定义有桶大小的hashmap, Key需定义operator size_t 函数
template<class Key, class Val, INT nBucketSize>
class CLockBucketMap
{
public:
    CLockBucketMap()
    {
        static_assert(nBucketSize> 0, "nBucketSize must big than zero!!!");
    }

    bool get_item(const Key& k, Val& v)
    {
        return m_buck_map[get_bucket_index(k)].get_item(k,v);
    }

    bool find_item(const Key& k)
    {
        return m_buck_map[get_bucket_index(k)].find_item(k);
    }

    bool add_item(const Key& k, const Val& v)
    {
        return m_buck_map[get_bucket_index(k)].add_item(k, v);
    }

    bool remove_item(const Key& k)
    {
        return m_buck_map[get_bucket_index(k)].remove_item(k);
    }

    bool update_item(const Key& k, const Val& v, bool bAdd)
    {
        return m_buck_map[get_bucket_index(k)].update_item(k, v, bAdd);
    }

    void for_each(std::function<void(const Key& k, const Val& v)> f)
    {
        for (int i=0; i<nBucketSize; ++i)
        {
            m_buck_map[i]->for_each(f);
        }
    }

protected:
    size_t get_bucket_index(const Key& k)
    {
        return ((size_t) k) % nBucketSize;
    }

protected:
    CLockMap<Key, Val>  m_buck_map[nBucketSize];
};





}}
