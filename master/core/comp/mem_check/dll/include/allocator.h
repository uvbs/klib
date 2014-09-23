#ifndef _klib_allocator_h_
#define _klib_allocator_h_


#include <string>


// ÄÚ´æ·ÖÅäÆ÷
template<typename T>
class MemAlloc : public std::allocator<T>
{  
public:  
    typedef size_t   size_type;  
    typedef typename std::allocator<T>::pointer              pointer;  
    typedef typename std::allocator<T>::value_type           value_type;  
    typedef typename std::allocator<T>::const_pointer        const_pointer;  
    typedef typename std::allocator<T>::reference            reference;  
    typedef typename std::allocator<T>::const_reference      const_reference;  
   
    pointer allocate(size_type _Count, const void* _Hint = NULL)
    {
        (_Hint);
        return (pointer) malloc(_Count * sizeof (T));
    }
  
    void deallocate(pointer _Ptr, size_type _Count)
    {
        (_Count);
        free(_Ptr);
    }
  
    template<class _Other>  
    struct rebind  
    {   // convert this type to allocator<_Other>  
        typedef MemAlloc<_Other> other;  
    };  
  
    MemAlloc() throw()  {}   
  
    /*MyAllc(const MyAllc& __a) throw()  
        : allocator<T>(__a)  {}*/
  
    template<typename _Tp1>
    MemAlloc(const MemAlloc<_Tp1>&) throw()  {}
  
    ~MemAlloc() throw()  {}
};



typedef std::basic_string<char, std::char_traits<char>, MemAlloc<char> > simp_string;


#endif  // _klib_allocator_h_