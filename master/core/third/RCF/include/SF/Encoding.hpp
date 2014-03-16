
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_ENCODING_HPP_
#define _SF_ENCODING_HPP_

#include <RCF/ByteOrdering.hpp>

#include <SF/DataPtr.hpp>
#include <SF/Exception.hpp>
#include <SF/Tools.hpp>

namespace SF {

    class Text;
    class BinaryNative;
    class BinaryPortable;

    static char chSeparator = ':';

    //************************************************************************

    template<typename EncodingT, typename T>
    class CountTypedElements;

    template<typename Encoding, typename T>
    class EncodeTypedElements;

    template<typename Encoding, typename T>
    class DecodeTypedElements;

    //************************************************************************

    template<typename T>
    class CountTypedElements<BinaryNative, T>
    {
    public:
        UInt32 operator()(DataPtr &data)
        {
            SF_ASSERT(data.length() % sizeof(T) == 0);
            return data.length() / sizeof(T);
        }
    };

    template<typename T>
    class CountTypedElements<BinaryPortable, T>
    {
    public:
        UInt32 operator()(DataPtr &data)
        {
            SF_ASSERT(data.length() % sizeof(typename PortableT<T>::Val) == 0);
            return data.length() / sizeof(typename PortableT<T>::Val);
        }
    };

    template<typename T>
    class CountTypedElements<Text, T>
    {
    public:
        UInt32 operator()(DataPtr &data)
        {
            // Count number of internally occuring separators in the data, and then add 1
            UInt32 count = 0;
            for (UInt32 i=1; i<data.length()-1; i++)
            {
                if (data.get()[i] == Byte8(chSeparator))
                {
                    count++;
                }
            }
            return count+1;
        }
    };

    template<>
    class CountTypedElements<Text, char>
    {
    public:
        UInt32 operator()(DataPtr &data)
        {
            return data.length();
        }
    };

    template<>
    class CountTypedElements<Text, unsigned char>
    {
    public:
        UInt32 operator()(DataPtr &data)
        {
            return data.length();
        }
    };

    //*******************************************************************************

    // TODO: precision issues when encoding/decoding floating point values?

    template<typename T>
    class EncodeTypedElements<Text, T>
    {
    public:
        void operator()(DataPtr &data, T *t, int nCount)
        {
            std::ostringstream ostr;
            ostr << t[0];
            for (int i=1; i<nCount; i++) 
            {
                ostr.put(chSeparator);
                ostr << t[i];
            }
            std::string s = ostr.str();
            data.assign(reinterpret_cast<const Byte8 *>(s.c_str()), static_cast<UInt32>(s.length()));
        }
    };

    template<typename T>
    class DecodeTypedElements<Text, T>
    {
    public:
        void operator()(DataPtr &data, T *t, int nCount)
        {
            if (data.length() == 0)
                SF_THROW(Exception, "Empty data block from input stream" );
            std::string strData(reinterpret_cast<char *>(data.get()), data.length());
            std::istringstream istr(strData);
            istr >> t[0];
            for (int i=1; i<nCount; i++) 
            {
                char ch;
                istr.get(ch);
                SF_ASSERT( ch == chSeparator );
                istr >> t[i];
            }
        }
    };

    template<>
    class EncodeTypedElements<Text, char>
    {
    public:
        void operator()(DataPtr &data, char *t, int nCount)
        {
            data.assign(reinterpret_cast<Byte8 *>(t), nCount);
        }
    };

    template<>
    class DecodeTypedElements<Text, char>
    {
    public:
        void operator()(DataPtr &data, char *t, int nCount)
        {
            memcpy(t, data.get(), nCount);
        }
    };

    template<>
    class EncodeTypedElements<Text, unsigned char>
    {
    public:
        void operator()(DataPtr &data, unsigned char *t, int nCount)
        {
            data.assign(reinterpret_cast<Byte8 *>(t), nCount);
        }
    };

    template<>
    class DecodeTypedElements<Text, unsigned char>
    {
    public:
        void operator()(DataPtr &data, unsigned char *t, int nCount)
        {
            memcpy(t, data.get(), nCount);
        }
    };

    // Binary encoding

    template<typename T>
    class EncodeTypedElements<BinaryNative, T>
    {
    public:
        void operator()(DataPtr &data, T *t, int nCount)
        {
            data.assign(reinterpret_cast<Byte8 *>(t), sizeof(T)*nCount );
        }
    };

    template<typename T>
    class DecodeTypedElements<BinaryNative, T>
    {
    public:
        void operator()(DataPtr &data, T *t, int nCount)
        {
            SF_ASSERT( data.length() == sizeof(T)*nCount);
            memcpy(t, data.get(), sizeof(T)*nCount);
        }
    };

    template<typename T>
    class EncodeTypedElements<BinaryPortable, T>
    {
    public:
        void operator()(DataPtr &data, T *t, int nCount)
        {
            typedef typename PortableT<T>::Val PortableT;
            UInt32 nBufferSize = sizeof(PortableT) * nCount;
            UInt32 nAlloc = data.allocate(nBufferSize);
            SF_ASSERT(nAlloc == nBufferSize);
            PortableT *buffer = reinterpret_cast<PortableT *>(data.get());
            // TODO: if T == PortableT, use a memcpy for the whole array
            for (int i=0; i<nCount; i++)
                buffer[i] = t[i];
            RCF::machineToNetworkOrder(buffer, sizeof(PortableT), nCount);
        }
    };

    template<typename T>
    class DecodeTypedElements<BinaryPortable, T>
    {
    public:
        void operator()(DataPtr &data, T *t, int nCount)
        {
            typedef typename PortableT<T>::Val PortableT;
            if (data.length() != sizeof(PortableT)*nCount)
                SF_THROW( Exception, "Stream format error while reading" )(data.length())(nCount)(typeid(PortableT).name());
            PortableT *buffer = reinterpret_cast<PortableT *>(data.get());
            RCF::networkToMachineOrder(buffer, sizeof(PortableT), nCount);
            // TODO: if T == PortableT, use a memcpy for the whole array
            for (int i=0; i<nCount; i++)
                t[i] = buffer[i];
        }
    };


    //*****************************************************************

    template<typename Encoding>
    class CountElements
    {
    public:
        UInt32 operator()(DataPtr &data, const std::type_info &objType)
        {

#define TRY_COUNT_ELEMENTS(type)                                    \
            if (objType == typeid(type))                            \
            {                                                       \
                return CountTypedElements<Encoding, type>()(data);  \
            }

            FOR_EACH_FUNDAMENTAL_TYPE( TRY_COUNT_ELEMENTS );

#undef TRY_COUNT_ELEMENTS                    

            SF_ASSERT(0)(objType);
            return 0;
        }
    };

    template<typename Encoding>
    class EncodeElements
    {
    public:
        void operator()(
            DataPtr &data, 
            void *pvObject, 
            const std::type_info &objType, 
            int nCount)
        {
            
#define TRY_ENCODE_ELEMENTS(type)                           \
            if (objType == typeid(type))                    \
            {                                               \
                EncodeTypedElements<Encoding, type>()(      \
                    data,                                   \
                    static_cast<type *>(pvObject),          \
                    nCount);                                \
                return;                                     \
            }

            FOR_EACH_FUNDAMENTAL_TYPE( TRY_ENCODE_ELEMENTS );

#undef TRY_ENCODE_ELEMENTS

            SF_ASSERT(0)(objType);

        }
    };

    template<typename Encoding>
    class DecodeElements
    {
    public:
        void operator()(
            DataPtr &data, 
            void *pvObject, 
            const std::type_info &objType, 
            int nCount)
        {

#define TRY_DECODE_ELEMENTS(type)                           \
            if (objType == typeid(type))                    \
            {                                               \
                DecodeTypedElements<Encoding, type>()(      \
                    data,                                   \
                    static_cast<type *>(pvObject),          \
                    nCount);                                \
                return;                                     \
            }

            FOR_EACH_FUNDAMENTAL_TYPE( TRY_DECODE_ELEMENTS );

#undef TRY_DECODE_ELEMENTS

            SF_ASSERT(0)(objType);
        }
    };

} // namespace SF


#endif // !_SF_ENCODING_HPP_
