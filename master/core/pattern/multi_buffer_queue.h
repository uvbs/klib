
#ifndef _CSDCS_MULTI_BUFFER_QUEUE_H_
#define _CSDCS_MULTI_BUFFER_QUEUE_H_

#include <queue>

#include "..\kthread\auto_lock.h"


typedef klib::kthread::mutex MutexType;
typedef klib::kthread::guard MutexGuardType;

/*
 多写单读队列
 MaxElement : 允许的最大存储个数
 QueueNum：   内部队列的个数
 ElemType:    要存储的类型
*/
template<size_t MaxElement, size_t QueueNum, typename ElemType, typename Container = std::queue<ElemType>>
class  multi_buffer_queue
{
public:
    multi_buffer_queue() : 
      read_index_(0),
      write_index_(0),
      element_count_(0)
    {
        size_t max_ele = MaxElement;
        size_t queue_num = QueueNum;
        size_t tmp = (max_ele % queue_num) > 0;  // 此句不能和下面一句合并
        max_queue_size_ = max_ele/queue_num + tmp;
        static_assert(QueueNum >= 2, "need more than 2 queue");
    }

    MutexType& mutex() { return mutex_; }

    size_t size() const { return element_count_; }

    bool push(const ElemType& elem)
    {
        MutexGuardType guard(mutex_);

        // 当前缓冲队列没有写满
        if (container_[write_index_].size() < max_queue_size_) 
        {
            ++ element_count_;
            container_[write_index_].push(elem);
        }
        else
        {
            // 写满后移动到下一个队列
            size_t next_write_index = (write_index_ + 1) % QueueNum;
            if (next_write_index == read_index_) 
            {
                return false;
            }

            ++ element_count_;
            write_index_ = next_write_index;
            container_[write_index_].push(elem);
        }

        return true;
    }

    bool pop(ElemType& e)
    {
        if (0 == element_count_) 
        {
            return false;
        }

        // 当前读索引为空的话，需要移动读索引
        if (container_[read_index_].empty()) 
        {
            ++ read_index_;
            read_index_ %= QueueNum;
            if (container_[read_index_].empty()) 
            {
                //CSASSERT(FALSE);
                return false;
            }
        }

        // 读取的索引一致的话，需要移动写的索引
        if (read_index_ == write_index_) 
        {
            MutexGuardType guard(mutex_);

            if (read_index_ == write_index_) 
            {
                write_index_ = (write_index_ + 1) % QueueNum;
            }
        }

        // 读取数据
        e = container_[read_index_].front();
        container_[read_index_].pop();

        // 减少数据计数
        MutexGuardType guard(mutex_);
        -- element_count_;

        return true;
    }

protected:
    MutexType       mutex_;                             // 互斥锁

    Container       container_[QueueNum];               // 队列定义
    size_t          max_queue_size_;                    // 每个队列的大小
    size_t          read_index_;                        // 读索引
    size_t          write_index_;                       // 写索引
    size_t          element_count_;                     // 元素的个数
};


#endif  // _CSDCS_MULTI_BUFFER_QUEUE_H_
