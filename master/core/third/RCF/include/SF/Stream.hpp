
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_STREAM_HPP_
#define _SF_STREAM_HPP_

#include <iosfwd>
#include <map>
#include <string>

#include <boost/noncopyable.hpp>

#include <SF/Archive.hpp>
#include <SF/DataPtr.hpp>
#include <SF/Encoding.hpp>
#include <SF/I_Stream.hpp>

// TODO: placed here for users convenience, maybe put it somewhere else
#include <SF/SerializeFundamental.hpp>
#include <SF/SerializeDynamicArray.hpp>
#include <SF/SerializeStaticArray.hpp>

namespace SF {

    //**************************************************
    // Encoding of object data

    template<typename E>
    class Encoding : public I_Encoding
    {
    public:
        virtual UInt32 getCount(DataPtr &data, const std::type_info &type)
        { 
            return CountElements<E>()(data, type); 
        }

        virtual void toData(DataPtr &data, void *pvObject, const std::type_info &type, int nCount)
        {
            EncodeElements<E>()(data, pvObject, type, nCount);
        }

        virtual void toObject(DataPtr &data, void *pvObject, const std::type_info &type, int nCount)
        { 
            DecodeElements<E>()(data, pvObject, type, nCount);
        }
    };

    template<typename E>
    class WithEncoding : public I_WithEncoding
    {
    public:
        virtual I_Encoding &getEncoding() { return e; }
    private:
        E e;
    };

    //**************************************************
    // Context handling

    // Context

    class ContextRead : public I_ContextRead
    {
    public:
        ContextRead();
        virtual ~ContextRead();
        virtual void add(SF::UInt32 nid, const ObjectId &id);
        virtual void add(void *ptr, const std::type_info &objType, void *pObj);
        virtual bool query(SF::UInt32 nid, ObjectId &id);
        virtual bool query(void *ptr, const std::type_info &objType, void *&pObj);
        virtual void clear();
    private:
        bool bEnabled_;
        std::map<UInt32, ObjectId> nid_id_;
        std::map<std::string, std::map< void *, void * > > type_ptr_obj_;
    };

    class ContextWrite : public I_ContextWrite
    {
    public:
        ContextWrite();
        virtual ~ContextWrite();
        virtual void setEnabled(bool enabled);
        virtual bool getEnabled();
        virtual void add(const ObjectId &id, UInt32 &nid);
        virtual bool query(const ObjectId &id, UInt32 &nid);
        virtual void clear();
    private:
        bool bEnabled_;
        UInt32 currentId_;
        std::map<ObjectId, UInt32> id_nid_;
    };

    class WithContextRead : public I_WithContextRead
    {
    public:
        virtual ~WithContextRead();
        virtual I_ContextRead &getContext();
    private:
        ContextRead context;
    };

    class WithContextWrite : public I_WithContextWrite
    {
    public:
        virtual ~WithContextWrite();
        virtual I_ContextWrite &getContext();
        void enableContext() { getContext().setEnabled(true); }
        void disableContext() { getContext().setEnabled(false); }
        void clearContext() { getContext().clear(); }
    private:
        ContextWrite context;
    };

    //**************************************************
    // Stream local storage

    class LocalStorage : public I_LocalStorage, boost::noncopyable
    {
    public:
        LocalStorage();
        virtual ~LocalStorage();
        virtual void set(const std::type_info &objType, void *pObj);
        virtual void *get(const std::type_info &objType);
    private:
        std::map< std::string, void * > type_ptr_;
    };

    class WithLocalStorage : public I_WithLocalStorage
    {
    public:
        virtual ~WithLocalStorage();
        virtual LocalStorage &getLocalStorage();
    private:
        LocalStorage localStorage_;
    };

    //****************************************************

    class WithSemanticsRead : public I_Stream
    {
    public:
        virtual ~WithSemanticsRead() {}

        // input dispatching

        template<typename T>
        WithSemanticsRead &operator>>(T &t)
        {
            Archive(Archive::READ, static_cast<I_Stream *>(this) ) & t;
            return *this;
        }

        template<typename T>
        WithSemanticsRead &operator>>(const T &t)
        {
            Archive(Archive::READ, static_cast<I_Stream *>(this) ) & t;
            return *this;
        }
        
    };

    class WithSemanticsWrite : public I_Stream
    {
    public:
        virtual ~WithSemanticsWrite() {}

        template<typename T>
        WithSemanticsWrite &operator<<(T &t)
        {
            Archive(Archive::WRITE, static_cast<I_Stream *>(this) ) & t;
            return *this;
        }

        template<typename T>
        WithSemanticsWrite &operator<<(const T &t)
        {
            Archive(Archive::WRITE, static_cast<I_Stream *>(this) ) & t;
            return *this;
        }

    };


    // stream base classes

    typedef Encoding<Text> EncodingText;
    typedef Encoding<BinaryNative> EncodingBinaryNative;
    typedef Encoding<BinaryPortable> EncodingBinaryPortable;

    typedef WithEncoding<EncodingText> WithEncodingText;
    typedef WithEncoding<EncodingBinaryNative> WithEncodingBinaryNative;
    typedef WithEncoding<EncodingBinaryPortable> WithEncodingBinaryPortable;

    class Node;

    class IStream :
        public WithSemanticsRead,
        public WithContextRead,
        public WithFormatRead,
        public WithLocalStorage,
        boost::noncopyable
    {
    public:
        IStream(std::istream &is);
    private:
        bool begin(Node &node);
        bool get(DataPtr &value);
        void end();
        UInt32 read_int(UInt32 &n);
        UInt32 read_byte(Byte8 &byte);
        UInt32 read(Byte8 *pBytes, UInt32 nLength);
        void putback_byte(Byte8 byte);
        std::istream &is;
    };

    class OStream :
        public WithSemanticsWrite,
        public WithContextWrite,
        public WithFormatWrite,
        public WithLocalStorage,
        boost::noncopyable
    {
    public:
        OStream(std::ostream &os);
    private:
        void begin(const Node &node);
        void put(const DataPtr &value);
        void end();
        UInt32 write_int(UInt32 n);
        UInt32 write_byte(Byte8 byte);
        UInt32 write_chararray(const Char8 *pChar, UInt32 nLength);
        UInt32 write(const Byte8 *pBytes, UInt32 nLength);
        std::ostream &os;
    };

} // namespace SF


#endif // !_SF_STREAM_HPP_
