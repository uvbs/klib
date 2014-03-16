
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_NODE_HPP_
#define _SF_NODE_HPP_

#include <boost/noncopyable.hpp>

#include <SF/DataPtr.hpp>
#include <SF/PortableTypes.hpp>

namespace SF {

    //****************************************************************************
    // Node class represents a node in the serialized hierarchy of objects 
    // (eg XML streams would translate it to an element in a DOM tree)

    class Node : boost::noncopyable
    {
    public:
        Node() : 
          type(), 
              label(), 
              id(), 
              ref()
          {}

          Node(const DataPtr &type, const DataPtr &label,  const UInt32 id, const UInt32 nullPtr) : 
          type(type), 
              label(label), 
              id(id), 
              ref(nullPtr)
          {}

          DataPtr type;
          DataPtr label;
          UInt32 id;
          UInt32 ref;
    };

} // namespace SF

#endif // ! _SF_NODE_HPP_
