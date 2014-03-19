
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_PORTABLETYPES_HPP_
#define _SF_PORTABLETYPES_HPP_

#include <boost/cstdint.hpp>
#include <boost/static_assert.hpp>

namespace SF {

    //***********************************************************************
    // Define some portable types (sizeof(...) will be same on all platforms)

    typedef bool                                Bool8;
    typedef char                                Char8;
    typedef unsigned char                       UChar8;
    typedef char                                Byte8;
    typedef float                               Float32;
    typedef double                              Double64;
    typedef boost::uint16_t                     Short16;
    typedef boost::int32_t                      Int32;
    typedef boost::uint32_t                     UInt32;
    
    BOOST_STATIC_ASSERT( sizeof(Bool8) == 1 );
    BOOST_STATIC_ASSERT( sizeof(Char8) == 1 );
    BOOST_STATIC_ASSERT( sizeof(UChar8) == 1 );
    BOOST_STATIC_ASSERT( sizeof(Byte8) == 1 );
    BOOST_STATIC_ASSERT( sizeof(Short16) == 2 );
    BOOST_STATIC_ASSERT( sizeof(Int32) == 4 );
    BOOST_STATIC_ASSERT( sizeof(UInt32) == 4 );
    BOOST_STATIC_ASSERT( sizeof(Float32) == 4 );
    BOOST_STATIC_ASSERT( sizeof(Double64) == 8 );

    template<typename T> struct PortableT;
    template<> struct PortableT<bool>            { typedef Bool8 Val; };
    template<> struct PortableT<char>            { typedef Char8 Val; };
    template<> struct PortableT<unsigned char>    { typedef Char8 Val; };
    template<> struct PortableT<short>            { typedef Short16 Val; };
    template<> struct PortableT<unsigned short>    { typedef Short16 Val; };
    template<> struct PortableT<int>            { typedef Int32 Val; };
    template<> struct PortableT<unsigned int>    { typedef Int32 Val; };
    template<> struct PortableT<long>            { typedef Int32 Val; };
    template<> struct PortableT<unsigned long>    { typedef Int32 Val; };
    template<> struct PortableT<float>            { typedef Float32 Val; };
    template<> struct PortableT<double>            { typedef Double64 Val; };
    template<> struct PortableT<long double>    { typedef Double64 Val; };

} // namespace SF

#endif // ! _SF_PORTABLETYPES_HPP_
