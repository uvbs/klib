#ifndef _klib_ptr_list_h_
#define _klib_ptr_list_h_


namespace klib {
namespace pattern {


/*
T must include T* next member 
*/

template<typename T>
class ptr_list
{
public:
    ptr_list() : 
        head_(nullptr),
        tail_(nullptr)
    {
    }

    size_t size() const { return size_; }

    bool push_back(T* t)
    {
        if (nullptr == t)
            return false;

        if (nullptr == tail_)
        {
            t->next = nullptr;
            head_ = tail_ = t;
        }
        else 
        {
            t->next = nullptr;
            tail_->next = t;
        }

        ++ size_;

        return true;
    }

    bool push_front(T* t)
    {
        if (nullptr == t)
            return false;

        if (nullptr == head_)
        {
            t->next = nullptr;
            head_ = tail_ = t;
        }
        else
        {
            t->next = head_;
            head_ = t;
        }

        ++ size_;
        return true;
    }

    T* front() const
    {
        if (0 == size())
            return nullptr;

        return head_;
    }

    void pop_front()
    {
        if (nullptr == head_)
            return;

        if (1 == size())
        {
            head_ = tail_ = nullptr;
        }
        else
        {
            head = head_->next;
        }
        -- size_;
    }

    bool remove(T* t)
    {
        T* tmp = head_;
        while (tmp)
        {
            if (tmp == t)
            {
                if (head_ == tmp)
                {
                    head_ = head_->next;
                }
                else 
                {
                    tmp = tmp->next;
                }
                break;
            }

            tmp = tmp->next;
        }
        return true;
    }

protected:
    size_t size_;
    T* head_;
    T* tail_;
};

}}



#endif  // _klib_ptr_list_h_