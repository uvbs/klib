#ifndef _klib_mem_seg_stream_h_
#define _klib_mem_seg_stream_h_


#include <vector>
#include <deque>


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

        t_host_type at(size_t index) 
        {
            if (index < t_fix_size) {
                return buff_[index];
            }
            throw std::exception();
        }

        pointer get() { return &buff_[0]; }

        bool write(const pointer p, size_t len)
        {
            if (write_pos_ + len > t_fix_size)
            {
                return false;
            }

            memcpy(&buff_[write_pos_], p, len);
            write_pos_ += len;
            return true;
        }

        bool read(pointer p, size_t len)
        {
            if ((t_fix_size - read_pos_) < len) {
                return false;
            }

            memcpy(p, &buff_[read_pos_], len);
            read_pos_ += len;
            return true;
        }

        size_t write_space() const {
            return t_fix_size - write_pos_;
        }

        size_t read_space() const {
            return t_fix_size - read_pos_;
        }

        bool write_eof()
        {
            if (write_pos_ == t_fix_size) {
                return true;
            }
            return false;
        }

        bool read_eof()
        {
            return (t_fix_size - read_pos_) == 0;
        }

    protected:
        size_t  write_pos_;
        size_t  read_pos_;
        t_host_type buff_[t_fix_size];
    };
}

//----------------------------------------------------------------------
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
    mem_seg_stream() : buff_len_(0),  r_seg_bf(0), w_seg_bf(0)
    {
    }
    
    int write(const buff_type bf, size_t len)
    {
        if (w_seg_bf == 0) 
        {
            if (!alloc_buffer()) {
                return -1;
            }
        }

        size_t writed_size = 0;
        size_t tmp_min_size;
        while (writed_size < len)
        {
            if (w_seg_bf->write_space() == 0) {
                if (!alloc_buffer()) {
                    buff_len_ += writed_size;
                    return writed_size;
                }
            }

            tmp_min_size = std::min<size_t>(w_seg_bf->write_space(), len - writed_size);
            w_seg_bf->write(bf + writed_size, tmp_min_size);
            writed_size += tmp_min_size;
        }
        buff_len_ += writed_size;
        return writed_size;
    }

    int read(buff_type bf, size_t len)
    {
        if (buff_len_ < len) {
            return 0;
        }

        if (r_seg_bf == 0) {
            r_seg_bf = buff_list_[0];
        }

        size_t readed_size = 0;
        size_t tmp_min_size;
        while (readed_size < len)
        {
            if (r_seg_bf->read_space() == 0) {
                free_buffer(r_seg_bf);
                r_seg_bf = buff_list_[0];
            }

            tmp_min_size = std::min<size_t>(len - readed_size, r_seg_bf->read_space());
            r_seg_bf->read(bf + readed_size, tmp_min_size);
            readed_size += tmp_min_size;
        }

        buff_len_ -= readed_size;
        return readed_size;
    }

    int peek(buff_type bf, size_t len)
    {
        if (buff_len_ < len) {
            return 0;
        }

        if (r_seg_bf == 0) {
            r_seg_bf = buff_list_[0];
        }

        size_t peek_pos = 0;
        size_t readed_size = 0;
        size_t tmp_min_size;
        while (readed_size < len)
        {
            if (r_seg_bf->read_space() == 0) {
                r_seg_bf = buff_list_[++ peek_pos];
            }

            tmp_min_size = std::min<size_t>(len - readed_size, r_seg_bf->read_space());
            r_seg_bf->read(bf + readed_size, tmp_min_size);
            readed_size += tmp_min_size;
        }

        buff_len_ -= readed_size;
        return readed_size;
    }

    size_t size() const
    {
        return buff_len_;
    }

protected:
    bool alloc_buffer()
    {
        seg_free_list_type* l = get_free_list();
        seg_buff_type* p = 0;
        if (!l->empty()) {
            p = l->front();
            l->pop_front();
        }

        if (0 == p) {
            p = new seg_buff_type;
        }

        if (0 == p) {
            return false;
        }

        buff_list_.push_back(p);
        w_seg_bf = p;

        return true;
    }

    bool free_buffer(seg_buff_type* p)
    {
        if (0 == p) {
            return false;
        }
        seg_free_list_type* l = get_free_list();
        l->push_back(p);

        buff_list_.pop_front();
        return true;
    }

    typedef std::list<seg_buff_type*> seg_free_list_type;
    seg_free_list_type* get_free_list()
    {
        static seg_free_list_type _the_seg_list;
        return &_the_seg_list;
    }

    

protected:
    size_t  buff_len_;

    seg_buff_type* w_seg_bf;
    seg_buff_type* r_seg_bf;

    std::deque<seg_buff_type*> buff_list_;
};




}}










#endif // _klib_mem_seg_stream_h_