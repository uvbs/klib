#ifndef _klib_ring_buffer_h_
#define _klib_ring_buffer_h_


#include "../kthread/mutex.h"

namespace klib {
namespace pattern {

typedef size_t     size_type;

/**
 * @brief  环型缓冲区
 * @tparam  TDataType  存储的类型
*/
template<class TDataType>
class ring_buffer
{
public:
    typedef ring_buffer<TDataType>              self_type;

public:
    ring_buffer() : max_size_(0), size_(0), write_pos_(0), read_pos_(0), data_arr_ptr_(NULL) {}
    ~ring_buffer() 
    {
        if (data_arr_ptr_) 
        {
            delete[] data_arr_ptr_;
        }
    }

    bool set_max_size(size_type nmax) 
    {
        if (nmax > 0 && max_size_ == 0) 
        {
            data_arr_ptr_ = new TDataType[nmax];
            max_size_ = nmax;
            if (NULL == data_arr_ptr_) 
            {
                max_size_ = 0;
            }
            return data_arr_ptr_ != NULL;
        }
        return true;
    }

    bool push_back(const TDataType& target)
    {
        if (size_ >= max_size_) 
            return false;

        _ASSERT(size_ <= max_size_);
        data_arr_ptr_[write_pos_] = target;
        ++ size_;
        ++ write_pos_;
        write_pos_ %= max_size_;

        return true;
    }

    bool push_force(const TDataType& target)
    {
        if (is_full()) 
        {
            pop_front();
        }
        return push_back(target);
    }
    
    bool front(TDataType& target)
    {
        if (0 == size_) 
            return false;

        target = data_arr_ptr_[read_pos_];

        return true;
    }

    bool pop_front()
    {
        if (0 == size_) 
            return false;

        ++ read_pos_;
        read_pos_ %= max_size_;
        -- size_;

        return true;
    }

    bool peek(TDataType& target) const
    {
        if (empty()) 
            return false;

        target = data_arr_ptr_[read_pos_];
        return true;
    }

    bool peek(TDataType*& pTarget) 
    {
        if (0 == size_) 
        {
            pTarget = NULL;
            return false;
        }

        pTarget = &data_arr_ptr_[read_pos_];
        return true;
    }

    bool peek_back(TDataType*& pTarget) 
    {
        if (empty()) 
        {
            return false;
        }

        size_type uIndex = size() - 1;
        pTarget = & at(uIndex);
        return true;
    }

    TDataType& at(size_type uIndex) 
    {
        size_type uTmp = read_pos_ + uIndex;
        uTmp %= max_size_;
        
        return data_arr_ptr_[uTmp];
    }

    size_type size() const
    {
        return size_;
    }

    bool is_full() const
    {
        return size_ == max_size_;
    }

    bool empty() const
    {
        return 0 == size_;
    }

	size_t max_size() const
	{
		return max_size_;
	}

protected:
    size_type max_size_;
    size_type size_;
    size_type write_pos_;
    size_type read_pos_;
    TDataType* data_arr_ptr_;
};


//----------------------------------------------------------------------
// 有锁缓冲区
template<class TDataType>
class lock_ring_buffer : public ring_buffer<TDataType>
{
    typedef ring_buffer<TDataType> parent_type;
public:
    bool set_max_size(size_type nmax) 
    {
        CCSAutoLock lock(&mutex_);

        return parent_type::set_max_size(nmax);
    }

    bool push_back(const TDataType& target)
    {
        CCSAutoLock lock(&mutex_);

        return parent_type::push_back(target);
    }

    bool front(TDataType& target)
    {
        CCSAutoLock lock(&mutex_);

        return parent_type::front(target);
    }

    bool pop_front()
    {
        CCSAutoLock lock(&mutex_);

        return parent_type::pop_front();
    }

    bool peek(TDataType& target) 
    {
        CCSAutoLock lock(&mutex_);

        return parent_type::peek(target);
    }

    bool peek(TDataType*& pTarget) 
    {
        CCSAutoLock lock(&mutex_);

        return parent_type::peek(pTarget);
    }

    bool peek_back(TDataType*& pTarget)  
    {
        CCSAutoLock lock(&mutex_);

        return parent_type::peek_back(pTarget);
    }

    TDataType at(size_type uIndex) 
    {
        guard lock(&mutex_);

        return parent_type::at(uIndex);
    }

    bool is_full() const 
    {
        return parent_type::is_full();
    }

protected:
    mutable klib::kthread::mutex mutex_;
};


}}


#endif // _klib_ring_buffer_h_