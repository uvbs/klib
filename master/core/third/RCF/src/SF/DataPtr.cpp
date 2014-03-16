
#include <SF/DataPtr.hpp>

#include <SF/Tools.hpp>

namespace SF {

    // TODO: need to be able to pass a preallocated buffer to DataPtr<T>, and use that if its big enough
    DataPtr::DataPtr() : 
        ptr_(buffer_), 
        length_(), 
        allocatedLength_(), 
        whichDeleter_(), 
        pfn_deleter_() 
    {}

    // TODO: ptr_ should never be NULL, so take action if the following 2 ctors recieve NULL pointers
    DataPtr::DataPtr(const T *sz) : 
        ptr_( const_cast<T*>(sz) ), 
        length_( length(sz) ), 
        allocatedLength_(length_+1), 
        whichDeleter_(0), 
        pfn_deleter_(NULL) 
    { 
        // Weak attach
        SF_ASSERT(ptr_); 
    }

    DataPtr::DataPtr(const T *sz, UInt32 length) : 
        ptr_( const_cast<T*>(sz) ), 
        length_(length), 
        allocatedLength_(length_), 
        whichDeleter_(), 
        pfn_deleter_(NULL) 
    { 
        // Weak attach
        SF_ASSERT(ptr_); 
    }

    DataPtr::DataPtr(const DataPtr &rhs) : 
        ptr_(rhs.ptr_), 
        length_(rhs.length_), 
        allocatedLength_(rhs.allocatedLength_), 
        whichDeleter_(0), 
        pfn_deleter_(NULL) 
    {
        // Weak copy
    } 

    DataPtr &DataPtr::operator=(const DataPtr &rhs) 
    {
        // Weak copy
        ptr_ = rhs.ptr_; 
        length_ = rhs.length_; 
        allocatedLength_ = rhs.allocatedLength_; 
        whichDeleter_ = 0; 
        pfn_deleter_ = NULL; 
        return *this; 
    } 
    
    DataPtr::~DataPtr() 
    { 
        release(); 
    }

    void DataPtr::assign(const T *sz, UInt32 length) 
    { 
        allocate(length); 
        memmove( get(), sz, length*sizeof(T) ); 
    }

    void DataPtr::assign(const T *sz) 
    { 
        assign(sz, length(sz)); 
    }
    
    void DataPtr::assign(const std::string &s) 
    { 
        assign(s.c_str(), s.length()); 
    }

    void DataPtr::release() 
    {
        switch (whichDeleter_) 
        {
        case 0: 
            break;
        case 1: 
            delete ptr_;  
            break;
        case 2: 
            delete [] ptr_; 
            break;
        case 3: 
            pfn_deleter_(ptr_); 
            break;
        default: 
            SF_ASSERT(0);
        }
        ptr_ = NULL; 
        length_ = 0; 
        whichDeleter_ = 0;
        pfn_deleter_ = NULL;
    }

    UInt32 DataPtr::allocate(UInt32 length) 
    { 
        release();
        if (length <= 63) 
        {
            ptr_ = buffer_;
            length_ = length;
            ptr_[length_] = 0;
            allocatedLength_ = length_ + 1;
            whichDeleter_ = 0;
            pfn_deleter_ = NULL;
        }
        else 
        {
            ptr_ = new T[length + 1];
            allocatedLength_ = length + 1;
            ptr_[length] = 0;
            length_ = length;
            whichDeleter_ = 2;
            pfn_deleter_ = NULL;
        }
        return length; 
    }

    void DataPtr::terminatebufferwithzero() const
    {
        if (ptr_ && ptr_[length_] != 0) 
        {
            if (allocatedLength_ > length_) 
            {
                ptr_[length_] = 0;
            }
            else if (allocatedLength_ == length_) 
            {
                // TODO: reallocate, this time with extra space for the trailing zero.
                SF_ASSERT(0);                
            }
        }
    }

    void DataPtr::update_length() 
    {
        if (ptr_)
            length_ = length(ptr_);
    }

    DataPtr::T *DataPtr::get() const 
    { 
        return ptr_; 
    }
    
    UInt32 DataPtr::length() const 
    { 
        return length_; 
    }
    
    bool DataPtr::empty() const 
    { 
        return length_ == 0; 
    }
    
    std::string DataPtr::cpp_str() const 
    { 
        return std::string(static_cast<char *>(ptr_), length_); 
    }

    UInt32 DataPtr::length(const T *sz)
    {
        if (sz) 
        {
            UInt32 i=0;
            while (sz[i])
                i++;
            return i;
        }
        else
            return 0;
    }

    bool operator<(const DataPtr &lhs, const DataPtr &rhs) 
    {
        typedef Byte8 T;
        SF_ASSERT(lhs.get());
        SF_ASSERT(rhs.get());
        if (lhs.length() < rhs.length())
            return true;
        else if (lhs.length() > rhs.length())
            return false;
        else {
            SF_ASSERT(lhs.length() == rhs.length());
            return memcmp(lhs.get(), rhs.get(), lhs.length()*sizeof(T)) < 0; 
        }
    }

    bool operator==(const DataPtr &lhs, const DataPtr &rhs) 
    { 
        typedef Byte8 T;
        SF_ASSERT(lhs.get());
        SF_ASSERT(rhs.get());
        return 
            lhs.length() == rhs.length() && 
            0 == memcmp(lhs.get(), rhs.get(), lhs.length()*sizeof(T)); 
    }

    bool operator!=(const DataPtr &lhs, const DataPtr &rhs) 
    { 
        return ! (lhs == rhs); 
    }

} // namespace SF
