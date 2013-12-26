#ifndef _klib_circular_buffer_h_
#define _klib_circular_buffer_h_

// #include <boost/cstdint.hpp>


/*
 循环缓冲区 
*/
template<size_t t_max_buff_size>
class circular_buffer
{
public:
    typedef unsigned char  char_type;
    typedef unsigned char* buffer_type;
    typedef unsigned int   segment_type;

#define MAX_SINGLE_VALUE (255)

    circular_buffer::circular_buffer() 
    {
        buffer_ = new unsigned char[t_max_buff_size];
        size_ = 0;

        write_pos_ = buffer_;
        read_pos_  = buffer_;
        end_pos_ = buffer_ + t_max_buff_size;
    }

    bool push(const buffer_type buff, size_t buff_len) 
    {
        // 判断空间够不
        if ((t_max_buff_size - size()) < get_room(buff_len)) 
        {
            return false;
        }

        if (buff_len >= MAX_SINGLE_VALUE) 
        {
            segment_type seg_len = (segment_type) buff_len;
            unsigned char ch = MAX_SINGLE_VALUE;
            write(ch);
            write(seg_len);
        }
        else 
        {
            unsigned char ch = (unsigned char) buff_len;
            write(ch);
        }

        return write(buff, buff_len);
    }

    bool pop(buffer_type buff, size_t buff_len)
    {
        // 判断缓冲区是否能装
        if (buff_len < next_size()) 
        {
            return false;
        }

        char_type ch;
        segment_type seg_len;
        size_t next_size_ = next_size();
        if (next_size_ >= MAX_SINGLE_VALUE) 
        {
            read(ch);
            read(seg_len);
        }
        else 
        {
            read(ch);
            seg_len = (segment_type) ch;
        }

        return read(buff, next_size_);
    }

    size_t next_size()  
    {
        if (size() == 0) 
        {
            return 0;
        }

        char_type ch;
        if (!peek(ch)) 
        {
            return 0;
        }

        if (ch < MAX_SINGLE_VALUE) 
        {
            return (size_t) ch;
        }

        segment_type seg_len;
        if (!peek(seg_len, sizeof(ch))) 
        {
            return 0;
        }

        return (size_t) seg_len; 
    }


    size_t size() const { return size_; }

protected:
    size_t get_room(size_t len) const 
    {
        if (len > MAX_SINGLE_VALUE) 
        {
            return (sizeof(char_type) + sizeof(segment_type) + len);
        }
        return (sizeof(char_type) + len);
    }

    bool peek(unsigned char* buff, int len) const
    {
        if (size() < len) 
        {
            return false;
        }

        auto pos_r = read_pos_;
        auto pos_w = buff;
        auto tmp_size = len;

        while (tmp_size > 0)
        {
            if (pos_r == end_pos_) 
            {
                pos_r = buffer_;
            }

            *pos_w = * pos_r;
            ++ pos_r;
            ++ pos_w;
            -- tmp_size;
        }

        return true;
    }

    bool peek(unsigned char& t, size_t off) const
    {
        if (size() < (sizeof(t) + off) )
        {
            return false;
        }

        auto pos_r = read_pos_ + off;
        auto pos_w = (buffer_type) &t;
        auto tmp_size = (int) sizeof(t);

        if (pos_r > end_pos_) 
        {
            pos_r = buffer_ + (pos_r - end_pos_);
        }

        while (tmp_size > 0)
        {
            if (pos_r == end_pos_) 
            {
                pos_r = buffer_;
            }

            *pos_w = * pos_r;
            ++ pos_r;
            ++ pos_w;
            -- tmp_size;
        }

        return true;
    }

    template<typename T>
    bool peek(T& t, size_t off = 0) const
    {
        if (size() < (sizeof(t) + off)) 
        {
            return false;
        }

        auto pos_r = read_pos_ + off;
        auto pos_w = (buffer_type) &t;
        auto tmp_size = (int) sizeof(t);

        if (pos_r > end_pos_) 
        {
            pos_r = buffer_ + (pos_r - end_pos_);
        }

        while (tmp_size > 0)
        {
            if (pos_r == end_pos_) 
            {
                pos_r = buffer_;
            }

            *pos_w = * pos_r;
            ++ pos_r;
            ++ pos_w;
            -- tmp_size;
        }

        return true;
    }

    template<typename T>
    bool read(T& t) 
    {
        if (size() < sizeof(t)) 
        {
            return false;
        }

        auto pos_r = read_pos_;
        auto pos_w = (buffer_type) &t;
        auto tmp_size = sizeof(t);

        while (tmp_size > 0)
        {
            if (pos_r == end_pos_) 
            {
                pos_r = buffer_;
            }

            *pos_w = * pos_r;
            ++ pos_r;
            ++ pos_w;
            -- tmp_size;
        }

        read_pos_ = pos_r;
        size_ -= sizeof(t);

        return true;
    }

    bool read(buffer_type buff, size_t& len)
    {
        if (size() < len) 
        {
            return false;
        }

        auto pos_w = buff;
        auto tmp_size = (int) len;

        while (tmp_size > 0)
        {
            if (read_pos_ == end_pos_) 
            {
                read_pos_ = buffer_;
            }

            * pos_w = * read_pos_;
            ++ pos_w;
            ++ read_pos_;
            -- tmp_size;
        }

        size_ -= len;

        return true;
    }

    template<typename T>
    bool write(const T& t)
    {
        if ((t_max_buff_size - size()) < sizeof(t)) 
        {
            return false;
        }

        auto pos_r = (buffer_type) &t;
        auto tmp_size = (int) sizeof(t);

        while (tmp_size > 0)
        {
            if (write_pos_ == end_pos_) 
            {
                write_pos_ = buffer_;
            }

            * write_pos_ = * pos_r;
            ++ write_pos_;
            ++ pos_r;
            -- tmp_size;
        }

        size_ += sizeof(t);

        return true;
    }

    bool write(const buffer_type buff, size_t buff_len)
    {
        if ((t_max_buff_size - size()) < buff_len) 
        {
            return false;
        }

        auto pos_r = (buffer_type) buff;
        auto tmp_size = (int) buff_len;

        while (tmp_size > 0)
        {
            if (write_pos_ == end_pos_) 
            {
                write_pos_ = buffer_;
            }

            * write_pos_ = * pos_r;
            ++ write_pos_;
            ++ pos_r;
            -- tmp_size;
        }

        size_ += buff_len;

        return true;
    }

protected:
    buffer_type  buffer_;
    buffer_type  write_pos_;
    buffer_type  read_pos_;
    buffer_type  end_pos_;
    size_t size_;

};


#endif //  _klib_circular_buffer_h_