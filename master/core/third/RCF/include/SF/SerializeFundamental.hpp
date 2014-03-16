
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_SERIALIZEFUNDAMENTAL_HPP_
#define _SF_SERIALIZEFUNDAMENTAL_HPP_

#include <SF/Archive.hpp>
#include <SF/DataPtr.hpp>
#include <SF/I_Stream.hpp>
#include <SF/Serializer.hpp>
#include <SF/Tools.hpp>

namespace SF {

    // serialize fundamental types

    template<typename T>
    inline void serializeFundamental(SF::Archive &ar, T &t, const unsigned int, unsigned int count = 1)
    {
        typedef typename boost::remove_const<T>::type U;
        BOOST_STATIC_ASSERT( boost::is_fundamental<U>::value );
        U *pu = const_cast<U *>(&t);
        void *pvt = pu;

        if (ar.isRead()) 
        {
            I_Encoding &encoding = dynamic_cast<I_WithEncoding *>(ar.getStream())->getEncoding();
            DataPtr data;
            dynamic_cast<WithFormatRead *>(ar.getStream())->get(data);
            if (count > 1 && count != encoding.getCount(data,typeid(U)) )
            {
                SF_THROW(SF::Exception, "Static array size mismatch" )(typeid(U).name())(count)(encoding.getCount(data,typeid(T)));
            }
            encoding.toObject(data, pvt, typeid(T), count );
        }
        else if (ar.isWrite()) 
        {
            I_Encoding &encoding = dynamic_cast<I_WithEncoding *>(ar.getStream())->getEncoding();
            DataPtr data;
            encoding.toData(data, pvt, typeid(U), count );
            dynamic_cast<WithFormatWrite *>(ar.getStream())->put(data);
        }
    }

#define DEFINE_FUNDAMENTAL_SERIALIZATION(type)                             \
    inline void serialize(SF::Archive &ar, type &t, unsigned int version)  \
    {                                                                      \
        serializeFundamental(ar, t, version);                              \
    }

    FOR_EACH_FUNDAMENTAL_TYPE( DEFINE_FUNDAMENTAL_SERIALIZATION )

#undef DEFINE_FUNDAMENTAL_SERIALIZATION


} // namespace SF

#endif // ! _SF_SERIALIZEFUNDAMENTAL_HPP_
