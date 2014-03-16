
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_SCOPED_PTR_HPP_
#define _SF_SCOPED_PTR_HPP_

//#include <boost/scoped_ptr.hpp>
namespace boost {
    template<class T> 
    class scoped_ptr;
}

#include <SF/SerializeSmartPtr.hpp>

namespace SF {

    // boost::scoped_ptr
    SF_SERIALIZE_SIMPLE_SMARTPTR( boost::scoped_ptr );

}

#endif // ! _SF_SCOPED_PTR_HPP_
