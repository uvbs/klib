
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_BYTEORDERING_HPP_
#define _RCF_BYTEORDERING_HPP_

#include <boost/static_assert.hpp>

#include <RCF/Tools.hpp>
#include <RCF/util/Platform/Platform.hpp>

namespace RCF {

    typedef Platform::Machine::LittleEndian         LittleEndian;
    typedef Platform::Machine::BigEndian            BigEndian;

    typedef BigEndian                               NetworkByteOrder;
    typedef Platform::Machine::ByteOrder            MachineByteOrder;

    inline void swapBytes(char *b1, char *b2) 
    {
        *b2 ^= *b1;
        *b1 ^= *b2;
        *b2 ^= *b1;
    }

    inline void reverseByteOrder(void *buffer, int width, int count)
    {
        RCF_ASSERT(width > 0);
        RCF_ASSERT(count > 0);
        if (width == 1) return;

        BOOST_STATIC_ASSERT( sizeof(char) == 1 );    
        char *chBuffer = static_cast<char *>(buffer);
        for (int i=0; i<count; i++)
            for (int j=0;j<width/2;j++)
                swapBytes( chBuffer + i*width + j, chBuffer + i*width + width - j - 1 );

    }

    template<typename From, typename To>
    struct Reorder;

    template<>
    struct Reorder<BigEndian, BigEndian>
    {
        void operator()(void *buffer, int width, int count) {}
    };

    template<>
    struct Reorder<LittleEndian, LittleEndian>
    {
        void operator()(void *buffer, int width, int count) {}
    };

    template<>
    struct Reorder<BigEndian, LittleEndian>
    {
        void operator()(void *buffer, int width, int count) { reverseByteOrder(buffer, width, count); }
    };

    template<>
    struct Reorder<LittleEndian, BigEndian>
    {
        void operator()(void *buffer, int width, int count) { reverseByteOrder(buffer, width, count); }
    };

    inline void machineToNetworkOrder(void *buffer, int width, int count)
    {
        Reorder<MachineByteOrder, NetworkByteOrder>()(buffer, width, count);
    }

    inline void networkToMachineOrder(void *buffer, int width, int count)
    {
        Reorder<NetworkByteOrder, MachineByteOrder>()(buffer, width, count);
    }

} // namespace RCF

#endif // ! _RCF_BYTEORDERING_HPP_
