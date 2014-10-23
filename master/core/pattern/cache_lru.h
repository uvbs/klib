#ifndef _klib_cache_lru_h_
#define _klib_cache_lru_h_


#include <list>
#include <vector>
#include <map>

namespace klib{
namespace pattern{


template <class K, class T>
struct cache_node
{
    cache_node() 
        : key(nullptr) 
        , prev(nullptr)
        , next(nullptr)
    {}

    K* key;
    T  data;
    cache_node *prev, *next;
};

template <class K, class T>
class cache_lru
{
    typedef std::map<K, cache_node<K,T>* >  cache_table_type;
public:
    cache_lru(size_t size)
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
    }

    ~cache_lru()
    {
        delete head_;
        delete tail_;
        delete[] entries_;
    }

    void put(K key, T data)
    {
        cache_node<K,T> *node = nullptr;

        cache_table_type::iterator itr = cache_tbl_.find(key);
        if (cache_tbl_.end() != itr) 
        {
            node = itr->second;
            detach(node);
            node->data = data;
            node->key  = (K*)&itr->first;
            attach(node);
        }
        else
        {
            if(free_entries_.empty())
            {// 可用结点为空，即cache已满
                node = tail_->prev;
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
                node->key = (K*)& itr.first->first;
            }
            attach(node);
        }
    }

    bool get(const K& key, T& t)
    {
        auto itr = cache_tbl_.find(key);
        if (itr == cache_tbl_.end()) 
        {
            return false;
        }
        else 
        {
            cache_node<K,T> *node = itr->second;
            detach(node);
            attach(node);
            t = node->data;
            return true;
        }
    }

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
    cache_table_type                  cache_tbl_;
    std::vector<cache_node<K,T>* >    free_entries_; // 存储可用结点的地址
    cache_node<K,T> *                 head_;
    cache_node<K,T> *                 tail_;
    cache_node<K,T> *                 entries_; // 双向链表中的结点
};


}}


#endif // 