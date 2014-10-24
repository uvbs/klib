#ifndef _klib_cache_lru_h_
#define _klib_cache_lru_h_


#include <list>
#include <vector>
#include <map>
#include <functional>


/*

// 
on_miss()
on_change()
on_detach()
脏数据在detach时存盘或改变时存盘

结合任务队列，异步存盘
*/

namespace klib{
namespace pattern{


template <class K, class T>
struct cache_node
{
    cache_node() 
        : dirty(false)
        , key(nullptr) 
        , prev(nullptr)
        , next(nullptr)
    {}
    
    bool       dirty;
    const K*   key;
    T          data;
    cache_node *prev, *next;
};

template <class K, class T>
class cache_lru
{
public:
    typedef cache_lru<K,T>  self_type;
    typedef std::map<K, cache_node<K,T>* >  cache_table_type;
    typedef std::function<bool(const K&, T*, self_type*)>             on_miss_callback;
    typedef std::function<void(const K&, T* /*old*/, T* /*new*/)>     on_change_callback;
    typedef std::function<void(const K& /*old key*/, const K& /*new key*/, 
        T* /*old val*/, T* /*new val*/)>     on_detach_callback;

public:
    cache_lru(size_t size);
    ~cache_lru();

    size_t size()       {  return cache_tbl_.size();  }
    size_t hit_count()  {  return hit_count_;         }
    size_t miss_count() {  return miss_count_;        }

    void put(const K& key, T data);
    bool get(const K& key, T& t);

    self_type& on_miss(on_miss_callback callback)
    { miss_callback_ = callback;  return * this; }

    self_type& on_change(on_change_callback callback)  
    { change_callback_ = callback; ;  return * this; }

    self_type& on_detach(on_detach_callback callback)  
    { detach_callback_ = callback; ;  return * this; }

private:
    // 分离结点
    void detach(cache_node<K,T>* node)
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    // 将结点插入头部
    void attach(cache_node<K,T>* node)
    {
        node->prev = head_;
        node->next = head_->next;
        head_->next = node;
        node->next->prev = node;
    }

private:
    cache_table_type                    cache_tbl_;
    std::vector<cache_node<K,T>* >      free_entries_;      // 存储可用结点的地址
    cache_node<K,T> *                   head_;
    cache_node<K,T> *                   tail_;
    cache_node<K,T> *                   entries_;           // 双向链表中的结点

    // callback;
    on_miss_callback                    miss_callback_;
    on_change_callback                  change_callback_;
    on_detach_callback                  detach_callback_;

    // 统计相关
    size_t                              miss_count_;
    size_t                              hit_count_;
};

template <class K, class T>
cache_lru<K,T>::cache_lru(size_t size)
{
    free_entries_.reserve(size);
    entries_ = new cache_node<K,T>[size];
    for(size_t index=0; index<size; ++index)// 存储可用结点的地址
        free_entries_.push_back(entries_ + index);

    head_ = new cache_node<K,T>;
    tail_ = new cache_node<K,T>;
    head_->prev = NULL;
    head_->next = tail_;
    tail_->prev = head_;
    tail_->next = NULL;

    miss_count_ = 0;
    hit_count_  = 0;
}

template <class K, class T>
cache_lru<K,T>::~cache_lru()
{
    delete      head_;
    delete      tail_;
    delete[]    entries_;
}

template <class K, class T>
void cache_lru<K,T>::put(const K& key, T data)
{
    cache_node<K,T> *node = nullptr;

    cache_table_type::iterator itr = cache_tbl_.find(key);
    if (cache_tbl_.end() != itr) 
    {
        node = itr->second;

        if (change_callback_)
            change_callback_(key, &node->data, &data);

        detach(node);
        node->data = data;
        node->key  = &itr->first;
        attach(node);
    }
    else
    {
        if(free_entries_.empty())
        {   // 可用结点为空，即cache已满
            node = tail_->prev;

            // 通知需要detach的元素
            if (detach_callback_)
                detach_callback_(*node->key, key, &node->data, &data);

            detach(node);
            size_t count = cache_tbl_.erase(* node->key);
            KLIB_ASSERT(1 == count);
        }
        else
        {
            node = free_entries_.back();
            free_entries_.pop_back();
        }

        node->data = data;
        // insert to map
        auto itr = cache_tbl_.insert(std::make_pair(key, node));
        if (itr.second) 
        {
            node->key = & itr.first->first;
        }
        attach(node);
    }
}

template <class K, class T>
bool cache_lru<K,T>::get(const K& key, T& t)
{
    auto itr = cache_tbl_.find(key);
    if (itr == cache_tbl_.end()) 
    {
        ++ miss_count_;

        if (miss_callback_)
            return miss_callback_(key, &t, this);

        return false;
    }
    else 
    {
        ++ hit_count_;

        cache_node<K,T> *node = itr->second;
        detach(node);
        attach(node);
        t = node->data;
        return true;
    }
}


}}


#endif // 