
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_MEMORY_HPP_
#define _SF_MEMORY_HPP_

#include <memory>

#include <SF/SerializeSmartPtr.hpp>

namespace SF {

    // serialize std::auto_ptr
    SF_SERIALIZE_SIMPLE_SMARTPTR( std::auto_ptr );

} // namespace SF

#endif // ! _SF_MEMORY_HPP_
