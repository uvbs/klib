#ifndef _klib_ref_buffer_h_
#define _klib_ref_buffer_h_


///
// 带引用计数的buff
// 需要添加membuffer 相关的方法 

template<size_t t_buf_len = 1024>
class ref_buffer
{
public:
    enum { e_buff_size = t_buf_len };

public:
    ref_buffer()
        : ref_count_(1)
    {}

    size_t add_ref() 
    {
        size_t formor_cont = ref_count_;
        ++ ref_count_; 
        return formor_cont;
    }

    void   release() 
    { 
        -- ref_count_; 
        if (0 == ref_count_) 
            delete this; 
    }

    size_t size() { return t_buf_len; }

    unsigned char* get_buffer()  { return &buff_[0];    }
    unsigned char* operator()()  { return get_buffer(); }

protected:
    size_t          ref_count_;

    unsigned char   buff_[t_buf_len];
};


class ref_buff
{
protected:
    ref_buff() 
        : ref_count_(1)
        , buff_size_(0)
    {}

public:
    static ref_buff* create(size_t bf_len)
    {
        size_t real_size = sizeof(ref_buff) + bf_len - sizeof(((ref_buff *)0)->buff_);
        unsigned char* p = new unsigned char[real_size];
        if (nullptr == p)
        {
            return nullptr;
        }

        ref_buff* ref_ptr = (ref_buff*) p;
        new (ref_ptr) ref_buff;

        ref_ptr->buff_size_ = bf_len;
        return ref_ptr;
    }

    size_t add_ref() 
    {
        size_t formor_cont = ref_count_;
        ++ ref_count_; 
        return formor_cont;
    }

    void   release() 
    { 
        -- ref_count_; 
        if (0 == ref_count_) 
            delete this;
    }
        
    size_t size() { return buff_size_; }

    unsigned char* get_buffer()  { return &buff_[0];    }
    unsigned char* operator()()  { return get_buffer(); }

protected:
    size_t          ref_count_;
    size_t          buff_size_;

    unsigned char   buff_[1];
};


template <class T>
class scope_buff
{
public:
    scope_buff(T* t) 
        : target_(t) 
    {
        t->add_ref();
    }

    ~scope_buff()   { target_->release(); } 

    operator T* ()
    {
        return target_;
    }

    T* operator -> ()
    {
        return target_;
    }

protected:
    T*  target_;
};





#endif // _klib_ref_buffer_h_