#pragma once


#include "mem_buffer.h"


namespace klib{
namespace mem{

template<int max_buffer_size, int min_block_size, typename T=char, typename Traits=std::char_traits<T> >
class basic_mem_string_base
{
    // µü´úÆ÷¶¨Òå
    template<typename T>
    struct buffer_pos
    {
        size_t m_pos;
        basic_mem_string_base* m_pbuffer;

        buffer_pos() {}
        buffer_pos(size_t pos, basic_mem_string_base* p) : m_pos(pos), m_pbuffer(p) {}

        buffer_pos& operator ++ ()
        {
            ++ m_pos;
            return *this;
        }
        buffer_pos operator ++ (int)
        {
            buffer_pos old_pos = *this;
            ++ m_pos;
            return old_pos;
        }

        T operator *() { return m_pbuffer->operator[](m_pos); }
        bool operator == (buffer_pos<T>& other) { return other.m_pos == this->m_pos; }
        bool operator !=  (buffer_pos<T>& other) { return other.m_pos != this->m_pos; }
    };

    typedef basic_mem_string_base<max_buffer_size, min_block_size, T, Traits> self_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef buffer_pos<T> pointer;
    typedef buffer_pos<T> const const_pointer;
    typedef buffer_pos<T>& reference;
    typedef buffer_pos<T> const& const_reference;

    typedef T value_type;
    typedef pointer iterator;
    typedef const_pointer const_iterator;

    typedef std::reverse_iterator<buffer_pos<T>>  const_reverse_iterator;
    typedef std::reverse_iterator<buffer_pos<T>>  reverse_iterator;
protected:
    typedef Traits traits_type;
public:
    size_type length()const
    {return m_buffer.size(); }
    size_type size() const
    {return m_buffer.size() / sizeof(T); }
    size_type max_size()const
    {return max_buffer_size; }
    size_type max_length()const
    {return max_buffer_size-1; }
    size_type capacity() const
    {return m_buffer.capacity(); }
    bool empty() const
    {return m_buffer.size() == 0; }
    T at(size_type pos)
    {
        if (m_buffer.size() <= pos)
            raise_range_error();

        return m_buffer.operator[](pos);
    }
    const_reference at(size_type pos) const
    {
        if (m_length <= pos)
            raise_range_error();
        return m_buffer.operator[](pos);
    }
    T operator[](size_type pos)
    {
        if (m_buffer.size() < pos)
            raise_range_error();
        return m_buffer.operator[](pos);
    }
    const_reference operator[](size_type pos) const
    {return m_buffer.operator[](pos); }

    // get iterators
    iterator begin()
    {
        pointer pt(0, this);
        return pt;
    }

    const_iterator begin() const
    {
        pointer pt(0, this);
        return pt;
    }
    iterator end()
    {
        pointer pt(m_buffer.size() == 0 ? 0 : m_buffer.size() , this);
        return pt; 
    }
    const_iterator end() const
    {
        pointer pt(m_buffer.size() == 0 ? 0 : m_buffer.size() , this);
        return pt; 
    }

    self_type& operator=(const self_type& src)
    {
        return assign(src);
    }
    self_type& operator=(const std::basic_string<T>& src)
    {
        return assign(src.c_str(),src.length());
    }
    self_type& operator=(const T * src)
    {
        return assign(src);
    }
    self_type& operator=(T ch)
    {
        return assign(1,ch);
    }
    bool operator == (self_type& other)
    {
        return m_buffer.operator== (other.m_buffer);
    }

    self_type& operator+=(const self_type& rhs)
    {return append(rhs); }
    self_type& operator+=(const T * rhs)
    {return append(rhs); }
    self_type& operator+=(T ch)
    {return append(1, ch); }

    operator std::basic_string<T>()const { return substr(0); }
    void clear() { m_buffer.clear(); }

    //////////////////////////////////////////////////////////////////
    // assign
    self_type& assign(const self_type& src)
    {
        if(this != &src)
            assign(src.c_str(), src.length()); 
        return *this;
    }

    template <typename Traits, typename A>
    self_type& assign(const std::basic_string<T,Traits,A>& src)
    {return assign(src.c_str(), src.length()); }

    self_type& assign(const self_type& src, size_type pos, size_type num)
    {
        if (src.length() < pos)
            raise_range_error();
        if(pos + num > src.length())
            num = src.length() - pos;
        return assign(src.c_str(),num);
    }

    template <typename Traits, typename A>
    self_type& assign(std::basic_string<T,Traits,A> const &src, size_type pos, size_type num)
    {
        if (src.length() < pos)
            raise_range_error();
        if(pos + num > src.length())
            num = src.length() - pos;
        return assign(src.c_str(),num);
    }

    self_type& assign(const T *src, size_type num)
    {
        m_buffer.clear();
        m_buffer.append((const char*) src, num * sizeof(T));
        return *this;
    }
    self_type& assign(const T *src)
    {return assign(src, traits_type::length(src)); }

    self_type& assign(size_type num, T ch)
    {
        if (num >= m_max_size)
            raise_range_error();
        num = num>m_max_size-1?m_max_size-1:num;
        traits_type::assign(m_pdata, num, ch);
        set_end_at(num);
        return *this; 
    }

    template<typename InputIterator>
    self_type& assign(InputIterator it_begin, InputIterator it_end)
    {return replace(begin(), end(), it_begin, it_end); }

    //////////////////////////////////////////////////////////////////
    // append
    self_type& append(const T *src, size_type length)
    {
        m_buffer.append((const char*) src, length * sizeof(T));
        return *this;
    }

    self_type& append(const T *src)
    {return append(src, traits_type::length(src));}

    self_type& append(self_type const &rhs)
    {
        m_buffer.append(rhs.m_buffer);
        return *this;
    }

    template <typename Traits, typename A>
    self_type& append(std::basic_string<T,Traits,A> const &rhs)
    {return append(rhs.c_str(),rhs.length());}

    self_type& append(size_type length, T ch)
    {
        size_type length_to_copy = 
            length>m_max_size-m_length-1?
            m_max_size-m_length-1
            ://else
        length;
        traits_type::assign(m_pdata + m_length, src, length_to_copy);
        set_end_at(m_length + length_to_copy);
        return *this; 
    }
    template<typename InputIterator>
    self_type& append(InputIterator it_begin, InputIterator it_end)
    {return replace(end(), end(), it_begin, it_end); }

    self_type& append(const self_type& src, size_type pos, size_type num)
    {
        if (src.length() < pos)
            raise_range_error();
        if(pos + num > src.length())
            num = src.length() - pos;
        return append(src.c_str() + pos, num);; 
    }

    template <typename Traits, typename A>
    self_type& append(std::basic_string<T,Traits,A> const &src, size_type pos, size_type num)
    {
        if (src.length() < pos)
            raise_range_error();
        if(pos + num > src.length())
            num = src.length() - pos;
        return append(src.c_str() + pos, num);
    }

    // copy
    size_type copy(T *dest, size_type num, size_type pos = 0) const
    {
        bool bfetech = m_buffer.copy(dest, pos, num);
        if (bfetech) 
            return num;
        return 0; 
    }

    std::basic_string<T> substr(size_type pos, size_type num) const
    {
        if (0 == num) 
            return std::basic_string<T>();

        auto str = std::basic_string<T>();

        size_t n_real_size = (num * sizeof(T)) > (m_buffer.size() - pos) ? (m_buffer.size() - pos) : num * sizeof(T);
        size_t n_char_size = n_real_size / sizeof(T);   // TCHAR length

        str.resize(n_char_size);
        m_buffer.copy((char*) str.c_str(), pos, n_real_size);
        
        return std::move(str); 
    }

    std::basic_string<T> substr(size_type pos = 0) const
    {
        return substr(pos, m_buffer.size() - pos);
    }

    //////////////////////////////////////////////////////////////////
    // compare
    int compare(const self_type& str) const
    {return compare(0, m_length, str.c_str(), str.size()); }
    template<typename StdStrTraits, typename A>
    int compare(const std::basic_string<T,StdStrTraits,A>& str) const
    {return compare(0, m_length, str.c_str(), str.size()); }
    int compare(size_type pos, size_type num, const self_type& str) const
    {return compare(pos, num, str.c_str(), str.length()); }
    int compare(size_type pos0, size_type num0, const self_type& str,
        size_type pos, size_type num) const
    {
        if (str.size() < pos)
            raise_range_error();
        if (str.length() - pos < num)
            num = str.m_length - pos;
        return compare(pos0, num0, str.c_str() + pos, num);
    }

    int compare(const T *str) const
    {return compare(0, m_length, str, traits_type::length(str)); }

    int compare(size_type pos, size_type num, const T *str) const
    {return compare(pos, num, str, traits_type::length(str)); }

    int compare(size_type pos, size_type num, const T *str, size_type len) const
    {
        if (m_length < pos)
            raise_range_error();
        if (m_length - pos < num)
            num = m_length - pos;
        size_type result = traits_type::compare(m_pdata + pos, str,
            num < len ? num : len);
        return (result != 0 ? result : num < len ? -1
            : num == len ? 0 : +1); 
    }

    // swap
    void swap(self_type& dest)
    {
        std::basic_string<T> t = *this; 
        *this = dest;
        dest = t; 
    }
    friend void swap(self_type& x, self_type& y)
    {x.swap(y); }

    // find*
    size_type find(const T *str, size_type pos, size_type num) const
    {
        if (num == 0 && pos <= m_buffer.size())
            return pos;

        size_type m = m_buffer.find((const char*) str, num * sizeof(T), pos* sizeof(T), true);
        return m / sizeof(T);
    }
    size_type find(const T *str, size_type pos = 0) const
    {return find(str, pos, traits_type::length(str)); }
    size_type find(T ch, size_type pos = 0) const
    {return find((const T *)&ch, pos, 1); }

    //
    bool is_begin_with(const T *str, size_type num) const
    {
        return m_buffer.is_begin_with(str, num);
    }
    bool is_end_with(const T *str, size_type num) const
    {
        return m_buffer.is_end_with(str, num);
    }

    basic_mem_string_base(){}

protected:
    void set_end_at(size_type pos)
    {
        //m_pdata[m_length = pos] = T(0);
    }
    static void raise_range_error()
    {
        throw std::out_of_range("invalid small_string<T> subscript");
    }
    static void raise_length_error()
    {
        throw std::length_error("length error in basic_small_string<T>");
    }

private:
    basic_mem_string_base(self_type const&);

protected:

    mem_buffer<max_buffer_size, min_block_size, FALSE> m_buffer;
};


typedef basic_mem_string_base<56*1024*1024, 32*1024>  mem_string;
typedef basic_mem_string_base<56*1024*1024, 32*1024, wchar_t, char_traits<wchar_t>>  mem_wstring;


}} 