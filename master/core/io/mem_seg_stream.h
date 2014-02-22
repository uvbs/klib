#ifndef _klib_mem_seg_stream_h_
#define _klib_mem_seg_stream_h_


#include <vector>
#include <deque>

#include "../core/klib_core.h"

namespace klib {
namespace io {

namespace detail
{
    template<typename t_host_type, int t_fix_size>
    class fix_stream
    {
    public:
        typedef t_host_type*  pointer;
    public:
        fix_stream() : write_pos_(0), read_pos_(0) {}
        
        void clear()
        {
            write_pos_ = 0;
            read_pos_  = 0;
        }

        pointer addr() { return &buff_[0]; }

        bool write(const pointer p, size_t len)
        {
            if (write_pos_ + len > t_fix_size)
                return false;

            memcpy(&buff_[write_pos_], p, len);
            write_pos_ += len;
            return true;
        }

        bool read(pointer p, size_t len)
        {
            if (size() < len) 
                return false;

            if (NULL != p) {
                memcpy(p, &buff_[read_pos_], len);
            }

            read_pos_ += len;
            return true;
        }
        
        pointer get_read_ptr()
        {
            return read_pos_ + (pointer) buff_;
        }

        // 获取还可以写的空间大小(就是写入的内容大小)
        size_t get_write_spare_space() const
        {
            return t_fix_size - write_pos_;
        }
        
        bool write_eof()
        {
            return write_pos_ == t_fix_size ;
        }

        bool read_eof()
        {
            return t_fix_size == read_pos_ ;
        }

        size_t size() const 
        {
            return write_pos_ - read_pos_;
        }

    protected:
        size_t  write_pos_;
        size_t  read_pos_;
        t_host_type buff_[t_fix_size];
    };
}

//----------------------------------------------------------------------
// buffer list structure
// t_seg_len : segment length
// t_max_mem_usage : max usage of the memory, 0 with out limit 
// t_host_type : buffer type
template<int t_seg_len, typename t_host_type = unsigned char, int t_max_mem_usage = 0>
class mem_seg_stream
{
public:
    typedef t_host_type  value_type;
    typedef t_host_type* buff_type;
    typedef detail::fix_stream<t_host_type, t_seg_len> seg_buff_type;

public:
    mem_seg_stream() : buff_len_(0)
    {
    }
    ~mem_seg_stream()
    {
        for (auto itr=buff_list_.begin(); itr != buff_list_.end(); ++itr)
        {
            delete (*itr);
        }
    }
    
    size_t write(const buff_type bf, size_t len)
    {
        seg_buff_type* w_seg_bf_ = get_write_buf();
        if (NULL == w_seg_bf_) {
            _ASSERT(false);
            return 0;
        }

        size_t writed_size = 0;
        size_t tmp_min_size;
        while (writed_size < len)
        {
            if (w_seg_bf_->get_write_spare_space() == 0) 
            {
                w_seg_bf_ = get_write_buf();
                if (NULL == w_seg_bf_) {
                    buff_len_ += writed_size;
                    return writed_size;
                }
            }

            tmp_min_size = std::min<size_t>(w_seg_bf_->get_write_spare_space(), len - writed_size);
            w_seg_bf_->write(bf + writed_size, tmp_min_size);
            writed_size += tmp_min_size;
        }
        buff_len_ += writed_size;

        verify();

        return writed_size;
    }

    size_t read(buff_type bf, size_t len)
    {
        if (buff_len_ < len)
            return 0;

        seg_buff_type* r_seg_bf_ = get_read_buf();
        if (NULL == r_seg_bf_) {
            _ASSERT(false);
            return 0;
        }

        size_t readed_size = 0;
        size_t tmp_min_size;
        while (readed_size < len)
        {
            tmp_min_size = std::min<size_t>(len - readed_size, r_seg_bf_->size());
            if (NULL == bf) {
                r_seg_bf_->read(NULL, tmp_min_size);
            }
            else  {
                r_seg_bf_->read(bf + readed_size, tmp_min_size);
            }

            readed_size += tmp_min_size;
            if (r_seg_bf_->read_eof()) 
            {
                free_first_buffer();
                r_seg_bf_ = get_read_buf();

                if (NULL == r_seg_bf_) {
                    break;
                }
            }
        }

        buff_len_ -= readed_size;

        verify();

        return readed_size;
    }

    value_type* get_read_seg_ptr() const
    {
        seg_buff_type* seg_bf = buff_list_[0];

        if (seg_bf->size() == 0) 
            seg_bf = buff_list_[1];

        return seg_bf->get_read_ptr();
    }

    size_t get_read_seg_len() const
    {
        seg_buff_type* seg_bf = buff_list_[0];

        if (seg_bf->size() == 0) 
            seg_bf = buff_list_[1];

        return seg_bf->size();
    }
    
    size_t size() const
    {
        return buff_len_;
    }

    void verify() const
    {
        size_t num = 0;
        for (auto itr = buff_list_.begin(); itr != buff_list_.end(); ++ itr)
        {
            num += (*itr)->size();
        }
        KLIB_ASSERT(num == buff_len_);
    }

protected:
    seg_buff_type* get_read_buf()
    {
        if (buff_list_.empty()) {
            return NULL;
        }

        return buff_list_[0];
    }

    seg_buff_type* get_write_buf()
    {
        seg_buff_type* p = NULL;
        if (buff_list_.empty()) {
            p = new seg_buff_type;
            if (NULL == p) {
                return p;
            }
            buff_list_.push_back(p);
            return p;
        }

        p = buff_list_[buff_list_.size() - 1];
        if (p->get_write_spare_space() == 0) {
            p = new seg_buff_type;
            if (NULL == p) {
                return p;
            }
            buff_list_.push_back(p);
            return p;
        }

        return p;
    }
    
    //@todo 修改原型
    bool free_first_buffer()
    {
        if (buff_list_.empty()) {
            _ASSERT(false);
            return false;
        }

        seg_buff_type* p = buff_list_.front();
        buff_list_.pop_front();
        delete p;
        
        return true;
    }

protected:
    size_t  buff_len_;
    
    std::deque<seg_buff_type*> buff_list_;
};




}}










#endif // _klib_mem_seg_stream_h_