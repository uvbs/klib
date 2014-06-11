
#ifndef _klib_noncopyable_h
#define _klib_noncopyable_h

namespace klib {
namespace core {


//----------------------------------------------------------------------
class noncopyable
{  
protected:  
    noncopyable() {}
    ~noncopyable() {}

private:  // emphasize the following members are private  
    noncopyable( const noncopyable& );  
    const noncopyable& operator=( const noncopyable& );
};


}}

#endif
