
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_SHARED_PTR_HPP_
#define _SF_SHARED_PTR_HPP_

//#include <boost/shared_ptr.hpp>
namespace boost {
    template<class T> 
    class shared_ptr;
}

#include <SF/SerializeSmartPtr.hpp>

namespace SF {

    // serialize boost::shared_ptr
    SF_SERIALIZE_REFCOUNTED_SMARTPTR( boost::shared_ptr );

} // namespace SF

#endif // ! _SF_SHARED_PTR_HPP_
