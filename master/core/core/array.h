//////////////////////////////// 
//array类和其的实现 

#ifndef _klib_array_h
#define _klib_array_h
/**
 * @brief 模板数组
 */

#include <cstddef> 
#include <iterator> 
#include <algorithm> 
#include <stdexcept> 


namespace klib { 
namespace core{

    template <typename T> 
    class array_base 
    { 
    public: 
        typedef T              value_type; 
        typedef T*             iterator; 
        typedef const T*       const_iterator; 
        typedef T&             reference; 
        typedef const T&       const_reference; 
        typedef /*std*/::size_t    size_type; 
        typedef /*std*/::ptrdiff_t difference_type; 
#ifdef _MSC_VER 
        typedef std::reverse_iterator<T> reverse_iterator; 
        typedef std::reverse_iterator<T> const_reverse_iterator; 
#else 
        typedef std::reverse_iterator<iterator> reverse_iterator; 
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator ; 
#endif 
    public: 
        iterator begin() { return pdata; } 
        const_iterator begin() const { return pdata; } 
        iterator end() { return pdata + size(); } 
        const_iterator end() const { return pdata + size(); } 
        reverse_iterator rbegin() { return reverse_iterator(end()); } 
        const_reverse_iterator rbegin() const { 
            return const_reverse_iterator(end()); 
        } 
        reverse_iterator rend() { return reverse_iterator(begin()); } 
        const_reverse_iterator rend() const { 
            return const_reverse_iterator(begin()); 
        } 
        // at() with range check 
        reference at(size_type i) { rangecheck(i); return pdata[i]; } 
        const_reference at(size_type i) const { rangecheck(i); return elems[ i];  } 
        // front() and back() 
        reference front() { return pdata[0]; } 
        const_reference front() const { return pdata[0]; } 
        reference back() { return pdata[size()-1]; } 
        const_reference back() const { return pdata[size()-1]; } 
        // size is constant 
        bool empty() { return pdata != 0; } 
        size_type max_size() { return size(); } 
        // swap (note: linear complexity) 
        //file://void swap (array<T,N>& y) { 
        //    std::swap_ranges(begin(),end(),y.begin()); 
        //file://} 
        // direct access to data 
        const T* data() const { return pdata; } 
        size_type size() const 
        { 
            int sum=1; 
            for(int i=0;i<dim_count;i++) 
                sum*=dims[i]; 
            return sum; 
        } 
        void assign (const T& value) 
        {
            std::fill(begin(), end(), value); 
        } 
        void swap (array_base<T>& y) 
        { 
            if(size() != y.size())throw std::length_error("array "); 
            std::swap_ranges(begin(),end(),y.begin()); 
        }

        template <typename T2> 
        array_base<T>& operator= (const array_base<T2>& rhs) 
        { 
            std::copy(rhs.begin(),rhs.end(), begin()); 
            return *this; 
        } 
        size_t get_dims(size_t index){return dims[i];} 
        void rangecheck (size_type i) 
        { 
            if (i >= size()) { throw std::range_error("array"); } 
        } 
    protected: 
        array_base(){} 
    protected: 
        T * pdata; 
        int dim_count; 
        int dims[3]; 
        array_base(int d):dim_count(d){} 
    }; 

    // comparisons 
    template<class T1,class T2> 
    bool operator== (const array_base<T1>& x, const array_base<T2>& y) { 

        return std::equal(x.begin(), x.end(), y.begin()); 
    } 
    template<class T> 
    bool operator< (const array_base<T>& x, const array_base<T>& y) { 
        return std::lexicographical_compare(x.begin(),x.end(),y.begin(),,y.end()); 
    } 
    template<class T1,class T2> 
    bool operator!= (const array_base<T1>& x, const array_base<T2>& y) { 

        return !(x==y); 
    } 
    template<class T1, class T2> 
    bool operator> (const array_base<T1>& x, const array_base<T2>& y) { 
        return y<x; 
    } 
    template<class T1, class T2> 
    bool operator<= (const array_base<T1>& x, const array_base<T2>& y) { 

        return !(y<x); 
    } 
    template<class T1, class T2> 
    bool operator>= (const array_base<T1>& x, const array_base<T2>& y) { 

        return !(x<y); 
    } 
    // global swap() 
    template<class T1, class T2> 
    inline void swap (array_base<T1>& x, array_base<T2>& y) { 
        x.swap(y); 
    } 
    template <typename T> 
    class array_1d_base : public array_base<T> 
    { 
    public: 
        const_reference operator [](size_type index)const 
        { 
            return pdata[index]; 
        } 
        reference operator[](size_type index) 
        { 
            return pdata[index]; 
        } 
        const_reference operator ()(size_type index)const 
        { 
            return pdata[index]; 
        } 
        reference operator()(size_type index) 
        { 
            return pdata[index]; 
        } 
    public: 
        array_1d_base(T* pd,size_type d1) 
        { 
            dim_count = 1; 
            pdata = pd; 
            dims[0] = d1; 
        } 
    protected: 
        array_1d_base() {dim_count=1;} 
    }; 
    template <typename T> 
    class array_2d_base : public array_base<T> 
    { 
    public: 
        const array_1d_base<T> operator[](size_type index)const 
        { 
            return array_1d_base<T>(pdata+index*dims[1],dims[1]) 
                ; 
        } 
        array_1d_base<T> operator[](size_type index) 
        { 
            return array_1d_base<T>(pdata+index*dims[1],dims[1]) 
                ; 
        } 
        //////////////////////////////////////////////////////////// 
        // 
        // Basic style access 
        const_reference operator ()(size_type index1, size_type index2) const 
        { 
            return pdata[index1 * dims[1] + index2]; 
        } 
        reference operator()(size_type index1, size_type index2) 
        { 
            return pdata[index1 * dims[1] + index2]; 
        } 
    public: 
        array_2d_base(T *pd,size_type d1,size_type d2) 
        { 
            dim_count = 2; 
            pdata   = pd; 
            dims[0] = d1; 
            dims[1] = d2; 
        } 
    protected: 
        array_2d_base() {dim_count = 2;} 
    }; 
    template <typename T> 
    class array_3d_base : public array_base<T> 
    { 
    public: 
        const array_2d_base<T> operator[](size_type index)const 
        { 
            return array_2d_base<T>( 
                pdata + index * dims[1] * dims[2], 
                dims[1], 
                dims[2]); 
        } 
        array_2d_base<T> operator[](size_type index) 
        { 
            return array_2d_base<T>( 
                pdata + index * dims[1] * dims[2], 
                dims[1], 
                dims[2] 
            ); 
        } 
        //////////////////////////////////////////////////////////// 
        // 
        // Basic style access 

        const_reference operator ()( 
            size_type index1, 
            size_type index2, 
            size_type index3)const 
        { 
            return pdata[index1 * dims[1] *dims[2] + 
                    index2 * dims[2] + 
                    index3]; 
        }

        reference operator()( 
            size_type index1, 
            size_type index2, 
            size_type index3) 
        { 
            return pdata[index1 * dims[1] *dims[2] + 
                    index2 * dims[2] + 
                    index3]; 
        } 
    protected: 
        array_3d_base() {dim_count=3;} 
    }; 

}}

namespace klib { 
namespace core {


    template <typename T, int dim1> 
    class array_1d : public array_1d_base<T> 
    { 
    public: 
        //CFC_STATIC_CONST(size_type, static_size = dim1); 
        const_reference operator[](size_type index)const 
        { 
            return data[index]; 
        } 
        reference operator[](size_type index) 
        { 
            return data[index]; 
        } 
        //////////////////////////////////////////////////////////// 
        // 
        // 
        const_reference operator()(size_type index)const 
        { 
            return data[index]; 
        } 
        reference operator()(size_type index) 
        { 
            return data[index]; 
        } 
    public: 
        array_1d() 
        { 
            pdata   = &data[0]; 
            dims[0] = dim1; 
        } 
    protected: 
        T data[dim1]; 
    }; 
    /////////////////////////////////////////////////////////////////// 
    // 2d 
    template <typename T, int dim1, int dim2> 
    class array_2d : public array_2d_base<T> 
    { 
#ifndef _MSC_VER // file://MSC doesn't support ref to a template array 
    private: 
        typedef T (&index_type)[dim2]; 
        typedef T const (&const_index_type)[dim2]; 
    public: 
        const_index_type operator[](size_type index)const 
        { 
            return data[index]; 
        } 
        index_type operator[](size_type index) 
        { 
            return data[index]; 
        } 
#endif 
    public: 
        //CFC_STATIC_CONST(size_type, static_size = dim1 * dim2); 
        const_reference operator()(size_type index1, size_type index2) const 
        { 
            return data[index1][index2]; 
        } 
        reference operator()(size_type index1, size_type index2) 
        { 
            return data[index1][index2]; 
        } 
    public: 
        array_2d() 
        { 
            pdata   = &data[0][0]; 
            dims[0] = dim1; 
            dims[1] = dim2; 
        } 
    private: 
        T data[dim1][dim2]; 
    }; 
    /////////////////////////////////////////////////////////////////// 
    // 3d 
    template <typename T, int dim1, int dim2, int dim3> 
    class array_3d : public array_3d_base<T> 
    { 
#ifndef _MSC_VER 
    private: 
        typedef T (&index_type)[dim2][dim3]; 
        typedef T const (&const_index_type)[dim2][dim3]; 
    public: 
        const_index_type operator[](size_type index)const 
        { 
            return data[index]; 
        } 
        index_type operator[](size_type index) 
        { 
            return data[index]; 
        } 
#endif 
    public: 
        //CFC_STATIC_CONST(size_type, static_size = dim1 * dim2 * dim3 ); 
        const_reference operator()( 
            size_type index1,
            size_type index2,
            size_type index3
            )const 
        { 
            return data[index1][index2][index2]; 
        } 
        reference operator()( 
            size_type index1, 
            size_type index2, 
            size_type index3) 
        { 
            return data[index1][index2][index3]; 
        } 
    public: 
        array_3d() 
        { 
            pdata   =&data[0][0][0]; 
            dims[0] = dim1; 
            dims[1] = dim2; 
            dims[2] = dim3; 
        }

    private: 
        T data[dim1][dim2][dim3]; 
    }; 

}}

#endif
