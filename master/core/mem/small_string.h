// small_string.hpp : Declares and defines the template class
// small_string and its base class and some helper classes and functions.
// Auther chen3feng, 
// Email chen3feng@163.com,chen3fengx@hotmail.com
// Date Oct 26,2002
// modify for klib(my develop library)

#ifndef _klib_SMALL_STRING_HPP_
#define _klib_SMALL_STRING_HPP_

#include <cstddef>
#include <string>
#include <iterator>

namespace klib {
namespace mem {


template<typename T, typename Traits=std::char_traits<T> >
class basic_small_string_base
{
public:
	typedef basic_small_string_base<T,Traits> self_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef T const* const_pointer;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T value_type;
	typedef pointer iterator;
	typedef const_pointer const_iterator;
// 	typedef std::reverse_iterator<const_iterator, 
//         value_type,
// 		const_reference, 
//         const_pointer, 
//         difference_type> const_reverse_iterator;
    typedef std::reverse_iterator<T> const_reverse_iterator;
// 	typedef std::reverse_iterator<iterator, value_type,
// 		reference, pointer, difference_type>
// 			reverse_iterator;
    typedef std::reverse_iterator<T>  reverse_iterator;
protected:
	typedef Traits traits_type;
public:
	size_type length()const
		{return m_length; }
	size_type size() const
		{return m_length; }
	size_type max_size()const
		{return m_max_size; }
	size_type max_length()const
		{return m_max_size-1; }
	size_type capacity() const
		{return m_max_size - 1; }
	const_pointer c_str()const
		{return m_pdata; }
	const_pointer data() const
		{return c_str(); }
	bool empty() const
		{return m_length == 0; }
	reference at(size_type pos)
	{
		if (m_length <= pos)
			raise_range_error();
		return m_pdata[pos]; 
	}
	const_reference at(size_type pos) const
	{
		if (m_length <= pos)
			raise_range_error();
		return m_pdata[pos];
	}
	reference operator[](size_type pos)
	{
		if (m_length < pos)
			raise_range_error();
		return m_pdata[pos];
	}
	const_reference operator[](size_type pos) const
		{return m_pdata[pos]; }
	// get iterators
	iterator begin()
		{return m_pdata; }
	const_iterator begin() const
		{return m_pdata; }
	iterator end()
		{return m_pdata + m_length; }
	const_iterator end() const
		{return m_pdata + m_length; }
	reverse_iterator rbegin()
		{return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const
		{return const_reverse_iterator(end()); }
	reverse_iterator rend()
		{return reverse_iterator(begin()); }
	const_reverse_iterator rend() const
		{return const_reverse_iterator(begin()); }
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

	self_type& operator+=(const self_type& rhs)
		{return append(rhs); }
	self_type& operator+=(const T * rhs)
		{return append(rhs); }
	self_type& operator+=(T ch)
		{return append(1, ch); }

	operator std::basic_string<T>()const
		{return std::basic_string<T>(m_pdata,m_length);}

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
		num = num > m_max_size-1 ? m_max_size-1:num;
		traits_type::copy(m_pdata, src, num);
		set_end_at(num);
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
		//if (npos - m_length <= _M)
		//	_Xlen();
		size_type length_to_copy = 
			length>m_max_size-m_length-1?
				m_max_size-m_length-1
			://else
				length;
		traits_type::copy(m_pdata + m_length, src, length_to_copy);
		m_length += length_to_copy;
		set_end_at(m_length); 
		return *this; 
	}

	self_type& append(const T *src)
		{return append(src,traits_type::length(src));}

	self_type& append(self_type const &rhs)
		{return append(rhs.c_str(),rhs.length());}

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
		return append(src.c_str() + pos, num);; 
	}

	// copy
	size_type copy(T *dest, size_type num, size_type pos = 0) const
	{
		if (m_length < pos)
			raise_range_error();
		if (m_length - pos < num)
			num = m_length - pos;
		if (num > 0)
			traits_type::copy(dest, m_pdata + pos, num);
		return num; 
	}

	////////////////////////////////////////////////////////////////
	// insert
	self_type& insert(size_type pos, const self_type& str)
		{return insert(pos, str.c_str(), str.length()); }

	self_type& insert(size_type pos1, const self_type& src, size_type pos2,	size_type num)
	{
		if(pos2>str.length())
			raise_range_error();
		if(num + pos1 > str.length())
			num = str.length() - pos1;
		return insert(pos1,str.c_str() + pos2, num); 
	}

	self_type& insert(size_type pos, const T *src, size_type num)
	{
		if (m_length < pos)
			raise_range_error();
		//if (npos - m_length <= _M)
		//	_Xlen();
		size_type length_to_copy = num;
		size_type length_to_move = m_length - pos;
		if(pos+num>m_max_size-1)
			length_to_copy= m_max_size-1-pos;
		if(length_to_move  > m_max_size-1 - (pos+length_to_copy))
			length_to_move = m_max_size-1 - (pos+length_to_copy);
		if(length_to_move)
			traits_type::move(m_pdata + pos + num, m_pdata + pos, length_to_move);
		if(length_to_copy)
			traits_type::copy(m_pdata + pos, src, length_to_copy);

		num = m_length + length_to_copy;
		if(num > m_max_size-1)
			num = m_max_size-1;
		set_end_at(num);

		return *this;
	}

	self_type& insert(size_type pos, const T *src)
	{
		return insert(pos, src, traits_type::length(src));
	}

	self_type& insert(size_type pos, size_type length, T ch)
	{
		if (m_length < pos)
			raise_range_error();
		size_type length_to_assign = length;
		size_type length_to_move = m_length - pos;
		if(pos+length>m_max_size-1)
			length_to_assign= m_max_size-1-pos;
		if(length_to_move  > m_max_size-1 - (pos+length_to_assign))
			length_to_move = m_max_size-1 - (pos+length_to_assign);
		traits_type::move(m_pdata + pos + length, m_pdata + pos, length_to_move);
		traits_type::assign(m_pdata + pos, length_to_assign, ch);
		m_length += length_to_assign;
		if(m_length>m_max_size-1)
			m_length = m_max_size-1;
		m_pdata[m_length] = 0;
		return *this;
	}

	iterator insert(iterator it, T ch)
	{
		size_type pos = it-begin();
		insert(pos, 1, ch);
		return begin() + pos; 
	}

	void insert(iterator it, size_type length, T ch)
	{
		size_type pos = it - begin();
		insert(pos, length, ch); 
	}

	template<typename InputIterator>
	void insert(iterator it_pos, InputIterator it_begin, InputIterator it_end)
	{
		replace(it_pos, it_pos, it_begin, it_end); 
	}

	///////////////////////////////////////////////////////////////////
	// erase
	self_type& erase(size_type pos, size_type length)
	{
		if (m_length < pos)
			raise_range_error();
		if (m_length - pos < length)
			length = m_length - pos;
		if (length > 0 )
		{
			traits_type::move(m_pdata + pos, m_pdata + pos + length,
				m_length - pos - length);
			m_length -= length;
			m_pdata[m_length] = 0;
		}
		return *this;
	}

	self_type& erase(size_type pos=0)
	{
		return erase(pos, length());
	}

	iterator erase(iterator it)
	{
		size_type pos = it -> begin();
		erase(pos, 1);
		return m_pdata + pos; 
	}

	iterator erase(iterator it_begin, iterator it_end)
	{
		size_type pos = it_begin - begin();
		erase(pos, it_end - it_begin);
		return m_pdata + pos; 
	}

	///////////////////////////////////////////////////////////////////
	// replace

	self_type& replace(size_type pos, size_type length, const self_type& src)
	{
		return replace(pos, length, src.c_str(), src.length());
	}

	template <typename Traits, typename A>
		self_type& replace(size_type pos, size_type length, const std::basic_string<T,Traits,A>& src)
	{
		return replace(pos, length, src.c_str(), src.length());
	}

	self_type& replace(size_type pos1, size_type length_to_replace, const self_type& str,
		size_type pos2, size_type num)
	{
		if(pos2>str.length())
			raise_range_error();
		if(num + pos1 > str.length())
			num = str.length() - pos1;
		return replace(pos1,length_to_replace,str.c_str() + pos2, num); 
	}

	template <typename Traits, typename A>
	self_type& replace(size_type pos1, size_type length_to_replace, const std::basic_string<T,Traits,A>& str,
		size_type pos2, size_type num)
	{
		if(pos2>str.length())
			raise_range_error();
		if(num + pos1 > str.length())
			num = str.length() - pos1;
		return replace(pos1,length_to_replace,str.c_str() + pos2, num); 
	}

	self_type& replace(size_type pos, size_type length_to_replace, const T *str, size_type num)
	{
		if (m_length < pos)
			raise_range_error();
		if (m_length - pos < length_to_replace)
			length_to_replace = m_length - pos;
		if(num > max_length()-pos)
			num = max_length()-pos;
		size_type length_to_move = m_length - (pos + length_to_replace);
		if(pos + num + length_to_move > max_length())
			length_to_move = max_length() - pos - num;
		if (length_to_move > 0)
			traits_type::move(m_pdata + pos + num, m_pdata + pos + length_to_replace, length_to_move);
		traits_type::copy(m_pdata + pos, str, num);

		int new_length = m_length + num - length_to_replace;
		if(new_length > max_length())
			new_length = max_length();

		set_end_at(new_length);

		return *this;
	}

	self_type& replace(size_type pos, size_type len, const T *src)
		{return replace(pos, len, src, traits_type::length(src)); }

	self_type& replace(size_type pos, size_type length_to_replace, size_type num, T ch)
	{
		if (m_length < pos)
			raise_range_error();
		if (m_length - pos < length_to_replace)
			length_to_replace = m_length - pos;
		if(num > max_length()-pos)
			num = max_length()-pos;
		size_type length_to_move = m_length - (pos + length_to_replace);
		if(pos + num + length_to_move > max_length())
			length_to_move = max_length() - pos - num;
		if (length_to_move > 0)
			traits_type::move(m_pdata + pos + num, m_pdata + pos + length_to_replace, length_to_move);
		traits_type::assign(m_pdata + pos, num, ch);

		int new_length = m_length + num - length_to_replace;
		if(new_length > max_length())
			new_length = max_length();

		set_end_at(new_length);

		return *this;
	}

	self_type& replace(iterator it_begin, iterator it_end, const self_type& str)
		{return replace(it_begin - begin(), it_end - it_begin, str); }

	template<typename Traits, typename A>
	self_type& replace(iterator it_begin, iterator it_end, const std::basic_string<T,Traits,A>& str)
		{return replace(it_begin - begin(), it_end - it_begin, str.c_str(), str.length()); }

	self_type& replace(iterator it_begin, iterator it_end, const T *str, size_type num)
		{return replace(it_begin - begin(), it_end - it_begin, str, num); }

	self_type& replace(iterator it_begin, iterator it_end, const T *str)
		{return replace(it_begin - begin(), it_end - it_begin, str); }

	self_type& replace(iterator it_begin, iterator it_end, size_type num, T ch)
		{return replace(it_begin - begin(), it_end - it_begin, num, ch); }

	template<typename InputIterator>
	self_type& replace(iterator it_begin1, iterator it_end1, InputIterator it_begin2, InputIterator it_end2)
	{
		size_type pos = it_begin1 - begin();
		size_type num = std::distance(it_begin2, it_end2);
			replace(pos, it_end1 - it_begin1, num, T(0));
		for (it_begin1 = begin() + pos; 0 < num; ++it_begin1, ++it_begin2, --num)
			*it_begin1 = *it_begin2;
		return *this;
	}

	std::basic_string<T> substr(size_type pos, size_type num) const
	{
		return std::basic_string<T>(m_pdata, pos, num); 
	}

	std::basic_string<T> substr(size_type pos = 0) const
	{
		return std::basic_string<T>(m_pdata, pos, m_length - pos); 
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
	size_type find(const self_type& x, size_type pos = 0) const
		{return find(x.c_str(), pos, x.size()); }
	size_type find(const T *str, size_type pos, size_type num) const
	{
		if (num == 0 && pos <= m_length)
			return pos;
		size_type m;
		if (pos < m_length && num <= (m = m_length - pos))
		{
			const T *u, *v;
			for (m -= num - 1, v = m_pdata + pos;
			(u = traits_type::find(v, m, *str)) != 0;
			m -= u - v + 1, v = u + 1)
				if (traits_type::compare(u, str, num) == 0)
					return u - m_pdata; 
		}
		return -1; 
	}
	size_type find(const T *str, size_type pos = 0) const
		{return find(str, pos, traits_type::length(str)); }
	size_type find(T ch, size_type pos = 0) const
		{return find((const T *)&ch, pos, 1); }
	size_type rfind(const self_type& str) const
		{return rfind(str.c_str(), length(), str.size()); }
	size_type rfind(const T *str, size_type pos, size_type num) const
	{
		if (num == 0)
			return (pos < m_length ? pos : m_length);
		if (num <= m_length)
		{
			for (const T * u = m_pdata +
				+ (pos < m_length - num ? pos : m_length - num); ; --u)
			{
				if (traits_type::eq(*u, *str)
					&& traits_type::compare(u, str, num) == 0)
					return u - m_pdata;
				else if (u == m_pdata)
					break;
			}
		}
		return -1; 
	}

	size_type rfind(const T *str, size_type pos) const
		{return rfind(str, pos, traits_type::length(str)); }

	size_type rfind(const T *str) const
		{return rfind(str, length(), traits_type::length(str)); }

	size_type rfind(T ch, size_type pos) const
		{return rfind((const T *)&ch, pos, 1); }

	size_type rfind(T ch) const
		{return rfind((const T *)&ch, length(), 1); }

	size_type find_first_of(const self_type& str, size_type pos = 0) const
		{return find_first_of(str.c_str(), pos, str.size()); }

	size_type find_first_of(const T *str, size_type pos, size_type num) const
	{
		if (0 < num && pos < m_length)
		{
			const T *const v = m_pdata + m_length;
			for (const T *u = m_pdata + pos; u < v; ++u)
			{
				if (traits_type::find(str, num, *u) != 0)
					return u - m_pdata;
			}
		}
		return -1; 
	}
	size_type find_first_of(const T *str, size_type pos = 0) const
		{return find_first_of(str, pos, traits_type::length(str)); }
	
	size_type find_first_of(T ch, size_type pos = 0) const
		{return find((const T *)&ch, pos, 1); }

	size_type find_last_of(const self_type& str, size_type pos) const
		{return find_last_of(str.c_str(), pos, str.size()); }

	size_type find_last_of(const self_type& str) const
		{return find_last_of(str.c_str(), length(), str.size()); }

	size_type find_last_of(const T *str, size_type pos, size_type num) const
	{
		if (0 < num && 0 < m_length)
		{
			for (const T *u = m_pdata
				+ (pos < m_length ? pos : m_length - 1); ; --u)
			{
				if (traits_type::find(str, num, *v) != 0)
					return u - m_pdata;
				else if (u == m_pdata)
					break;
			}
		}
		return -1; 
	}

	size_type find_last_of(const T *str, size_type pos) const
		{return find_last_of(str, pos, traits_type::length(str)); }

	size_type find_last_of(const T *str) const
		{return find_last_of(str, length(), traits_type::length(str)); }

	size_type find_last_of(T ch, size_type pos) const
		{return rfind((const T *)&ch, pos, 1); }

	size_type find_last_of(T ch) const
		{return rfind((const T *)&ch, length(), 1); }

	size_type find_first_not_of(const self_type& str, size_type pos = 0) const
	{
		return find_first_not_of(str.c_str(), pos, str.size()); 
	}
	size_type find_first_not_of(const T *str, size_type pos,
		size_type num) const
	{
		if (pos < m_length)
		{
			const T *const v = m_pdata + m_length;
			for (const T *u = m_pdata + pos; u < v; ++u)
				if (traits_type::find(str, num, *u) == 0)
					return u - m_pdata; 
		}
		return -1; 
	}

	size_type find_first_not_of(const T *str, size_type pos = 0) const
		{return find_first_not_of(str, pos, traits_type::length(str)); }

	size_type find_first_not_of(T ch, size_type pos = 0) const
		{return find_first_not_of((const T *)&ch, pos, 1); }

	size_type find_last_not_of(const self_type& str, size_type) const
		{return find_last_not_of(str.c_str(), pos, str.size()); }

	size_type find_last_not_of(const self_type& str) const
		{return find_last_not_of(str.c_str(), length(), str.size()); }

	size_type find_last_not_of(const T *str, size_type pos, size_type num) const
	{
		if (0 < m_length)
		{
			for (const T *u = m_pdata
				+ (pos < m_length ? pos : m_length - 1); ; --u)
			{
				if (traits_type::find(str, num, *u) == 0)
					return u - m_pdata;
				else if (u == m_pdata)
					break;
			}
		}
		return -1;
	}

	size_type find_last_not_of(const T *str, size_type pos) const
		{return find_last_not_of(str, pos, traits_type::length(str)); }

	size_type find_last_not_of(const T *str) const
		{return find_last_not_of(str, length(), traits_type::length(str)); }

	size_type find_last_not_of(T ch, size_type pos) const
		{return find_last_not_of((const T *)&ch, pos, 1); }

	size_type find_last_not_of(T ch) const
		{return find_last_not_of((const T *)&ch, length(), 1); }

protected:
	basic_small_string_base(T * p,size_type size)
		:m_pdata(p), m_max_size(size)
	{
	}
	void set_end_at(size_type pos)
	{
		m_pdata[m_length = pos] = T(0);
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
	basic_small_string_base();
	basic_small_string_base(self_type const&);
protected:
	size_type const m_max_size;
	size_type m_length;
	T *m_pdata;
};

//////////////////////////////////////////////////////////////////////
// small_string operations
//////////////////////////////////////////////////////////////////////


// operator+

template<class T, class Traits>
inline std::basic_string<T, Traits>  operator+(
										 const basic_small_string_base<T,Traits>& lhs,
										 const basic_small_string_base<T,Traits>& rhs)
{
	return std::basic_string<T, Traits>(lhs) += rhs;
}

template<class T, class Traits, class StdStrTraits, class A>
inline std::basic_string<T, Traits, A>  operator+(
										 const basic_small_string_base<T,Traits>& lhs,
										 const std::basic_string<T, StdStrTraits, A>& rhs)
{
	return std::basic_string<T, Traits, A>(lhs) += rhs;
}

template<class T, class Traits, class StdStrTraits, class A>
inline std::basic_string<T, Traits, A>  operator+(
										 const std::basic_string<T, StdStrTraits, A>& lhs,
										 const basic_small_string_base<T,Traits>& rhs)
{
	return std::basic_string<T, Traits, A>(lhs) += rhs;
}

template<class T, class Traits>
inline std::basic_string<T>  operator+(
	const T *lhs,
	const basic_small_string_base<T,Traits>& rhs)
{
	return basic_string<T>(lhs) += rhs;
}

template<class T, class Traits> 
inline std::basic_string<T> operator+(
	const T lhs, const basic_small_string_base<T,Traits>& rhs)
{
	return basic_string<T>(1, lhs) += rhs;
}
template<class T, class Traits> 
inline std::basic_string<T>  operator+(
	const basic_small_string_base<T,Traits>& lhs,
	const T *rhs)
{
	return std::basic_string<T>(lhs) += rhs;
}

template<class T, class Traits> 
inline std::basic_string<T>  operator+(
	const basic_small_string_base<T,Traits>& lhs, const T rhs)
{
	return std::basic_string<T>(lhs) += rhs;
}


// operator==

template<class T, class Traits> 
inline bool  operator==(const basic_small_string_base<T,Traits>& lhs,
						const basic_small_string_base<T,Traits>& rhs)
{
	return lhs.compare(rhs) == 0;
}

template<class T, class Traits, class StdStrTraits, class A>
inline bool  operator==(const std::basic_string<T,StdStrTraits,A>& lhs,
						const basic_small_string_base<T,Traits>& rhs)
{
	return lhs.compare(rhs, rgs.length()) == 0;
}

template<class T, class Traits, class StdStrTraits, class A>
inline bool  operator==(const basic_small_string_base<T,Traits>& lhs,
						const std::basic_string<T,StdStrTraits,A>& rhs)
{
	return lhs.compare(rhs, rhs.length()) == 0;
}

template<class T, class Traits> 
inline bool  operator==(const T * lhs,
						const basic_small_string_base<T,Traits>& rhs)
{
	return rhs.compare(lhs) == 0; 
}

template<class T, class Traits>
inline bool  operator==(const basic_small_string_base<T,Traits>& lhs,
						const T *rhs)
{
	return lhs.compare(rhs) == 0; 
}


// operator!=

template<class T, class Traits>
inline bool  operator!=(const basic_small_string_base<T,Traits>& lhs,
						const basic_small_string_base<T,Traits>& rhs)
{
	return !(lhs == rhs);
}

template<class T, class Traits, class StdStrTraits, class A>
inline bool  operator!=(const std::basic_string<T,StdStrTraits,A>& lhs,
						const basic_small_string_base<T,Traits>& rhs)
{
	return !(lhs == rhs);
}

template<class T, class Traits, class StdStrTraits, class A>
inline bool  operator!=(const basic_small_string_base<T,Traits>& lhs,
						const std::basic_string<T,StdStrTraits,A>& rhs)
{
	return !(lhs == rhs);
}

template<class T, class Traits>
inline bool  operator!=(
	const T *lhs,
	const basic_small_string_base<T, Traits>& rhs)
{
	return !(lhs == rhs); 
}

template<class T,class Traits>
inline bool  operator!=(const basic_small_string_base<T,Traits>& lhs,
						const T *rhs)
{
	return !(lhs == rhs);
}


// operator<

template<class T,class Traits>
inline bool  operator<(const basic_small_string_base<T,Traits>& lhs,
					   const basic_small_string_base<T,Traits>& rhs)
{
	return lhs.compare(rhs) < 0; 
}

template<class T, class Traits, class StdStrTraits, class A>
inline bool  operator<(const std::basic_string<T,StdStrTraits,A>& lhs,
						const basic_small_string_base<T,Traits>& rhs)
{
	return lhs.compare(rhs.c_str(), rhs.length()) < 0; 
}

template<class T, class Traits, class StdStrTraits, class A>
inline bool  operator<(const basic_small_string_base<T,Traits>& lhs,
						const std::basic_string<T,StdStrTraits,A>& rhs)
{
	return lhs.compare(rhs) < 0;
}

template<class T,class Traits>
inline bool  operator<(const T * lhs,
					   const basic_small_string_base<T,Traits>& rhs)
{
	return rhs.compare(lhs) > 0;
}
template<class T, class Traits>
inline bool  operator<(const basic_small_string_base<T,Traits>& lhs,
					   const T *rhs)
{
	return lhs.compare(rhs) < 0;
}


// operator>

template<class T, class Traits>
inline bool  operator>(const basic_small_string_base<T,Traits>& lhs,
					   const basic_small_string_base<T,Traits>& rhs)
{
	return rhs < lhs;
}

template<class T, class Traits, class StdStrTraits, class A>
inline bool  operator>(const std::basic_string<T,StdStrTraits,A>& lhs,
						const basic_small_string_base<T,Traits>& rhs)
{
	return rhs < lhs;
}

template<class T, class Traits, class StdStrTraits, class A>
inline bool  operator>(const basic_small_string_base<T,Traits>& lhs,
						const std::basic_string<T,StdStrTraits,A>& rhs)
{
	return rhs < lhs;
}

template<class T, class Traits> 
inline bool  operator>(const T * lhs,
					   const basic_small_string_base<T,Traits>& rhs)
{
	return rhs < lhs; 
}
template<class T, class Traits>
inline bool  operator>(const basic_small_string_base<T,Traits>& lhs,
					   const T *rhs)
{
	return rhs < lhs;
}


// operator<=

template<class T, class Traits>
inline bool  operator<=(const basic_small_string_base<T,Traits>& lhs,
						const basic_small_string_base<T,Traits>& rhs)
{
	return !(rhs < lhs);
}

template<class T, class Traits, class StdStrTraits, class A>
inline bool  operator<=(const std::basic_string<T,StdStrTraits,A>& lhs,
						const basic_small_string_base<T,Traits>& rhs)
{
	return !(rhs < lhs);
}

template<class T, class Traits, class StdStrTraits, class A>
inline bool  operator<=(const basic_small_string_base<T,Traits>& lhs,
						const std::basic_string<T,StdStrTraits,A>& rhs)
{
	return !(rhs < lhs);
}

template<class T, class Traits>
inline bool  operator<=(const T * lhs,
						const basic_small_string_base<T,Traits>& rhs)
{
	return !(rhs < lhs);
}

template<class T, class Traits>
inline bool  operator<=(const basic_small_string_base<T,Traits>& lhs,
						const T *rhs)
{
	return !(rhs < lhs);
}


// operator>=

template<class T, class Traits>
inline bool  operator>=(const basic_small_string_base<T,Traits>& lhs,
						const basic_small_string_base<T,Traits>& rhs)
{
	return !(lhs < rhs);
}

template<class T, class Traits, class StdStrTraits, class A>
inline bool  operator>=(const std::basic_string<T,StdStrTraits,A>& lhs,
						const basic_small_string_base<T,Traits>& rhs)
{
	return !(lhs < rhs);
}

template<class T, class Traits, class StdStrTraits, class A>
inline bool  operator>=(const basic_small_string_base<T,Traits>& lhs,
						const std::basic_string<T,StdStrTraits,A>& rhs)
{
	return !(lhs < rhs);
}

template<class T, class Traits>
inline bool  operator>=(const T * lhs,
						const basic_small_string_base<T,Traits>& rhs)
{
	return !(lhs < rhs); 
}


template<class T, class Traits>
inline bool  operator>=(const basic_small_string_base<T,Traits>& lhs,
						const T *rhs)
{
	return !(lhs < rhs);
}

// basic_small_string inserters and extractors
template<class T, class Traits,class StmTraits>
inline std::basic_istream<T, StmTraits>&  operator>>(
	std::basic_istream<T, Traits>& is,
	basic_small_string_base<T,Traits>& dest)
{
	std::basic_string<T,Traits> str;
	is >> str;
	dest = str;
	return is;
}

template<class T, class Traits, class StmTraits>
inline std::basic_istream<T, Traits>&  getline(std::basic_istream<T, StmTraits>& is,
									  basic_small_string_base<T, Traits>& dest)
{
	bsaic_string<T,StmTraits> str;
	getline(is, str, is.widen('\n'));
	dest = str;
	return is;
}

template<class T, class Traits, class StmTraits>
inline std::basic_istream<T, Traits>&  getline(std::basic_istream<T, StmTraits>& is,
									  basic_small_string_base<T,Traits>& dest, const T ch)
{
	basic_string<T,StmTraits> str;
	getline(is, str, ch);
	dest = str;
    return is;
}

template<class T, class Traits, class StmTraits>
inline std::basic_ostream<T, StmTraits>&  operator<<(
		std::basic_ostream<T, StmTraits>& os,
        const basic_small_string_base<T, Traits>& src)
{
    os << src.c_str();
    return os; 
}


//////////////////////////////////////////////////////////////////////
// template class basic_small_string declared and defined 
//

template<typename T, size_t MaxSize, typename Traits=std::char_traits<T> >
class basic_small_string : public basic_small_string_base<T,Traits>
{
	typedef basic_small_string_base<T,Traits> base_type;
public:
	enum{static_max_size=MaxSize};
	size_type length()const
	{
		return m_length;
	}
protected:
	basic_small_string() : basic_small_string_base<T,Traits>(m_data,MaxSize)
	{
		m_data[0]=0;
		m_length = 0;
	}

	basic_small_string(basic_small_string_base<T,Traits> const & src)
		:basic_small_string_base<T,Traits>(m_data,MaxSize)
	{
		assign(src);
	}

	basic_small_string(T const * src) 
		:basic_small_string_base<T,Traits>(m_data,MaxSize)
	{
		assign(src);
	}

	basic_small_string(T const * src, size_type length) 
		:basic_small_string_base<T,Traits>(m_data,MaxSize)
	{
		assign(src,length);
	}

	basic_small_string(size_type length, T ch)
		:basic_small_string_base<T,Traits>(m_data,MaxSize)
	{
		assign(length, ch);
	}

private:
	T m_data[MaxSize];
};


typedef basic_small_string_base<char, std::char_traits<char> > 
	small_string_base;

typedef basic_small_string_base<wchar_t, std::char_traits<wchar_t> > 
	small_wstring_base;

//////////////////////////////////////////////////////////////////////
// class small_string, the ansi version of small_string
//////////////////////////////////////////////////////////////////////

// 模板不支持typedef,我就得把基类的很多东西照抄一遍,天理难容啊!!!
template <size_t MaxSize, typename Traits=std::char_traits<char> >
class small_string : public basic_small_string<char, MaxSize,Traits>
{
	typedef basic_small_string<char,MaxSize,Traits> base_type;
	typedef typename base_type::value_type value_type;
public:
	small_string(){}

	small_string(basic_small_string_base<char,Traits> const& src)
		:base_type(src)
	{
	}

	small_string(char const * src) : base_type(src)
	{
	}

	small_string(base_type::size_type length, char ch)
		:base_type(length,ch)
	{
	}

	template<typename A>
	small_string& operator=(std::basic_string<value_type, char, A> const &src)
	{
		basic_small_string_base<value_type, Traits>::operator=(src);
		return *this;
	}
};

template <size_t MaxSize, typename Traits=std::char_traits<wchar_t> >
class small_wstring : public basic_small_string<char, MaxSize, Traits>
{
	typedef basic_small_string<char,MaxSize,Traits> base_type;
public:
	small_wstring()
	{
	}

	small_wstring(basic_small_string_base<typename base_type::value_type,Traits> const& src)
		:base_type(src)
	{
	}

	small_wstring(base_type::const_pointer src):base_type(src)
	{
	}

	small_wstring(typename base_type::size_type length, typename base_type::value_type ch)
		:base_type(length,ch)
	{
	}

};

}} // end namespace cfc


#endif // _SMALL_STRING_HPP_