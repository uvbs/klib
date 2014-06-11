
#ifndef _klib_buffer_reader_h
#define _klib_buffer_reader_h


#include <functional>

namespace klib {
namespace util {

typedef std::function<bool (unsigned char)> skip_callbcak;

class buffer_reader
{
public:
    typedef const unsigned char* ptr_type;
    typedef buffer_reader self_type;

public:
    buffer_reader() { set_buff((ptr_type) 0, 0); }
    buffer_reader(const unsigned char* buff, size_t len) { set_buff(buff, len); }

    void set_buff(const unsigned char* buff, size_t len);
    void set_buff(const char* buff, size_t len);

    ptr_type get_ptr() const { return cur_ptr; }
    size_t   size() const { return end_ptr - cur_ptr; }

    template<typename T>
    bool read(T&t);
    bool read(unsigned char* buff, size_t len);

    bool parser_value(size_t& val, size_t base);

    bool back(size_t len);
    bool skip(size_t len);
    bool skip(skip_callbcak callback);
    bool skip_until(unsigned char ch);
    bool skip_space();    

    self_type& operator ++ ();
    self_type& operator ++ (int);

protected:
    const unsigned char*  end_ptr;
    const unsigned char*  cur_ptr;

    const unsigned char*  buff_ptr;
    size_t                buff_len;
};



template<typename T>
bool buffer_reader::read(T&t)
{
    if (cur_ptr + sizeof(t) > end_ptr) 
    {
        return false;
    }

    t = *(T*) cur_ptr;
    cur_ptr += sizeof(t);
    return true;
}

}}

#endif      // _klib_buffer_reader_h

