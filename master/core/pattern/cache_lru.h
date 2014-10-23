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
    K key;
    T data;
    cache_node *prev, *next;
};

template <class K, class T>
class cache_lru
{
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
        cache_node<K,T> *node = cache_tbl_[key];
        if(node)
        { // node exists
            detach(node);
            node->data = data;
            attach(node);
        }
        else
        {
            if(free_entries_.empty())
            {// 可用结点为空，即cache已满
                node = tail_->prev;
                detach(node);
                cache_tbl_.erase(node->key);
            }
            else
            {
                node = free_entries_.back();
                free_entries_.pop_back();
            }

            node->key = key;
            node->data = data;
            cache_tbl_[key] = node;
            attach(node);
        }
    }

    bool get(K key, T& t)
    {
        cache_node<K,T> *node = cache_tbl_[key];
        if(node)
        {
            detach(node);
            attach(node);
            t = node->data;
            return true;
        }
        else
        {   // 如果cache中没有，返回false
            return false;
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
    std::map<K, cache_node<K,T>* >    cache_tbl_;
    std::vector<cache_node<K,T>* >    free_entries_; // 存储可用结点的地址
    cache_node<K,T> *                 head_;
    cache_node<K,T> *                 tail_;
    cache_node<K,T> *                 entries_; // 双向链表中的结点
};


}}


#endif // 