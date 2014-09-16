#ifndef _klib_lock_stl_h
#define _klib_lock_stl_h


#include "../kthread/auto_lock.h"

#include <functional>
#include <map>
#include <hash_map>

namespace klib {
namespace stl {

using namespace klib::kthread;

typedef auto_lock MutexGuard;
typedef auto_cs   Mutex;

//----------------------------------------------------------------------
// 有锁链接基类
template<class T, class _Container>
class lock_list_base
{
public:
    typedef lock_list_base<T, _Container>  self_type;
    typedef _Container  ContainerType;

public:
    size_t size() const
    {
        MutexGuard lock(mutex_);

        return container_.size();
    }

    bool find_item(const T& t)
    {
        MutexGuard lock(mutex_);

        auto itr = container_.begin();
        for (; itr != container_.end(); ++itr)
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
        MutexGuard lock(mutex_);

        container_.push_back(t);
        return true;
    }
    
    bool pop_item(T& t)
    {
        MutexGuard lock(mutex_);

        if (container_.empty()) 
        {
            return false;
        }

        t = container_.front();
        container_.pop_front();
        return true;
    }

    bool remove_item(T& t)
    {
        MutexGuard lock(mutex_);

        container_.remove(t);
        return true;
    }

    void for_each(std::function<bool(T& t)> f)
    {
        MutexGuard lock(mutex_);

        auto itr = container_.begin();
        for (; itr != container_.end(); ++ itr)
        {
            if (!f(*itr))  break;
        }
    }
    
    _Container& get_container()
    {
        return container_;
    }

    Mutex& get_lock()
    {
        return mutex_;;
    }

protected:
    mutable Mutex mutex_;;                  ///< 锁
    _Container container_;               ///< 容器
};

//----------------------------------------------------------------------
// 有锁链接
template<class T>
class lock_list : public lock_list_base<T, std::list<T>>
{
public:
    typedef lock_list<T>  self_type;
    typedef std::list<T>   ContainerType;

};

//----------------------------------------------------------------------
// 有锁vector基类
template<class T, class _Container>
class lock_vector_base
{
public:
    size_t size() const
    {
        MutexGuard lock(mutex_);

        return container_.size();
    }

    bool find_item(const T& t)
    {
        MutexGuard lock(mutex_);

        auto itr = container_.begin();
        for (; itr != container_.end(); ++itr)
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
        MutexGuard lock(mutex_);

        auto itr = container_.begin();
        for (; itr != container_.end(); ++itr)
        {
            if (*itr == t)
            {
                container_.erase(itr);
                return true;
            }
        }

        return false;
    }

    bool push_item(const T& t)
    {
        MutexGuard lock(mutex_);

        container_.push_back(t);
        return true;
    }
    
    bool pop_item(T& t)
    {
        MutexGuard lock(mutex_);

        if (container_.empty()) 
        {
            return false;
        }

        t = container_.front();
        container_.erase(container_.begin());
        return true;
    }

    void for_each(std::function<void(T& t)> f)
    {
        MutexGuard lock(mutex_);

        auto itr = container_.begin();
        for (; itr != container_.end(); ++ itr)
        {
            f(*itr);
        }
    }

    _Container& get_container()
    {
        return container_;
    }


    Mutex& get_lock()
    {
        return mutex_;;
    }

protected:
    mutable Mutex mutex_;;        ///< 临界区
    _Container container_;       ///< 容器
};

//----------------------------------------------------------------------
// 有锁vector
template<class T>
class lock_vector : public lock_vector_base<T, std::vector<T>>
{
public:

};


//----------------------------------------------------------------------
// 有锁stack
template<class T>
class lock_stack 
{
public:
    bool push_item(const T& t)
    {
        MutexGuard lock(mutex_);

        container_.push_back(t);
        return true;
    }

    bool pop_item(T& t)
    {
        MutexGuard lock(mutex_);

        if (container_.empty()) 
        {
            return false;
        }

        t = container_.back();
        container_.pop_back();
        return true;
    }

    size_t size()
    {
        return container_.size();
    }

    Mutex& get_lock()
    {
        return mutex_;;
    }

protected:
    Mutex     mutex_;;
    std::vector<T>  container_;
};


//----------------------------------------------------------------------
// 锁对基类
template<class Key, class Val, class _Container>
class lock_pair_base
{
public:
    ///<  获取大小
    size_t size() const
    {
        return container_.size();
    }

    ///< 获取键值对中的值
    bool get_item(const Key& k, Val& v)
    {
        MutexGuard lock(mutex_);

        auto itr = container_.find(k);
        if (itr != container_.end()) 
        {
            v = itr->second;
            return true;
        }
        return false;
    }

    ///< 查找是否存在该值
    bool find_item(const Key& k)
    {
        MutexGuard lock(mutex_);

        auto itr = container_.find(k);
        if (itr == container_.end())
        {
            return false;
        }
        return true;
    }

    ///< 添加一项
    bool add_item(const Key& k, const Val& v)
    {
        MutexGuard lock(mutex_);

        auto itr = container_.find(k);
        if (itr != container_.end()) 
        {
            return false;
        }

        container_.insert(std::make_pair(k, v));
        return true;
    }

    ///< 更新item
    bool update_item(const Key& k, const Val& v, BOOL bAdd)
    {
        MutexGuard lock(mutex_);

        auto itr = container_.find(k);
        if (itr == container_.end()) 
        {
            if (!bAdd) 
            {
                return false;
            }
            
            container_.insert(std::make_pair(k, v));
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
        MutexGuard lock(mutex_);

        container_.erase(k);

        return true;
    }

    ///< 根据条件删除项,matchfun 可以对v进行释放操作
    void remove_if(std::function<bool(const Key& k, Val& v)> matchfun)
    {
        MutexGuard lock(mutex_);

        auto itr = container_.begin();
        for (; itr != container_.end(); )
        {
            if (matchfun(itr->first, itr->second))
            {
                itr = container_.erase(itr);
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
        MutexGuard lock(mutex_);

        auto itr = container_.find(k);
        if (itr != container_.end()) 
        {
            checkfun(itr->second);
        }

        return;
    }

    ///< 释放其中的一项,会调用delete
    bool delete_item(const Key& k)
    {
        MutexGuard lock(mutex_);

        auto itr = container_.find(k);
        if (itr != container_.end()) 
        {
            delete itr->second;
        }
        container_.erase(itr);
        return true;
    }

    ///< 提供遍历的接口
    void for_each(std::function<void(const Key& k, const Val& v)> f)
    {
        MutexGuard lock(mutex_);

        auto itr = container_.begin();
        for (; itr != container_.end(); ++ itr)
        {
            f(itr->first, itr->second);
        }
    }

protected:
    _Container container_;
    mutable Mutex mutex_;;
};

//----------------------------------------------------------------------
// LockMap
template<class Key, class Val>
class lock_map : public lock_pair_base<Key, Val, std::map<Key, Val>>
{
public:    
};

//----------------------------------------------------------------------
// LockHashMap
template<class Key, class Val>
class lock_hash_map : public lock_pair_base<Key, Val, stdext::hash_map<Key, Val>>
{
public:
};

//----------------------------------------------------------------------
// 定义有桶大小的hashmap, Key需定义operator size_t 函数
template<class Key, class Val, INT nBucketSize>
class lock_bucket_map
{
public:
    lock_bucket_map()
    {
        static_assert(nBucketSize> 0, "nBucketSize must big than zero!!!");
    }

    size_t size()
    {
        size_t num_ = 0;
        for (size_t i=0; i<nBucketSize; ++i)
        {
            num_ += m_buck_map[i].size();
        }
        return num_;
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
    lock_map<Key, Val>  m_buck_map[nBucketSize];
};





}}


#endif
