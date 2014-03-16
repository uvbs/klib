
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_SERIALIZER_HPP_
#define _SF_SERIALIZER_HPP_

#include <SF/Archive.hpp>
#include <SF/I_Stream.hpp>
#include <SF/Registry.hpp>
#include <SF/SerializePolymorphic.hpp>
#include <SF/SfNew.hpp>
#include <SF/Tools.hpp>


namespace boost {
    namespace serialization {
        template<class Base, class Derived>
        const Base & base_object(const Derived & d);
    }
}

namespace SF {

    // Generic serializer, subclassed by all other serializers

    class SerializerBase : boost::noncopyable
    {
    private:
        bool invokeRead(Archive &ar);
        bool invokeWrite(Archive &ar);

        // Following are overridden to provide type-specific operations.
        virtual std::string getTypeName() = 0;
        virtual void newObject(Archive &ar) = 0;
        virtual bool isDerived() = 0;
        virtual std::string getDerivedTypeName() = 0;
        virtual void getSerializerPolymorphic(const std::string &derivedTypeName) = 0;
        virtual bool invokeSerializerPolymorphic(SF::Archive &) = 0;
        virtual void serializeContents(Archive &ar) = 0;
        virtual void addToInputContext(I_Stream *, const UInt32 &) = 0;
        virtual bool queryInputContext(I_Stream *, const UInt32 &) = 0; 
        virtual void addToOutputContext(I_Stream *, UInt32 &) = 0;
        virtual bool queryOutputContext(I_Stream *, UInt32 &) = 0;
        virtual void setFromId() = 0;
        virtual void setToNull() = 0;
        virtual bool isNull() = 0;
        virtual bool isNonAtomic() = 0;

    public:
        SerializerBase();
        virtual ~SerializerBase();
        bool invoke(Archive &ar);
    };

    //---------------------------------------------------------------------
    // Type-specific serializers

    template<typename T>
    class Serializer : public SerializerBase 
    {
    public:
        Serializer(T **ppt) :
            ppt(ppt), 
            pf(),
            id()
        {}

    private:
        typedef ObjectId IdT;
        T **ppt;
        I_SerializerPolymorphic *pf;
        IdT id;

        std::string getTypeName()
        {
            return SF::Registry::getSingleton().template getTypeName<T>();
        }

        void newObject(Archive &ar) 
        {
            *ppt = sfNew((T *) NULL, (T **) NULL, ar); 
        }

        bool isDerived() 
        {
            if (*ppt && typeid(T) != typeid(**ppt)) 
            {
                if (!SF::Registry::getSingleton().isTypeRegistered( typeid(**ppt) ))
                    SF_THROW(SF::Exception, "type not registered")(typeid(T))(typeid(**ppt));
                return true;
            }
            return false;
        }

        std::string getDerivedTypeName() 
        { 
            return SF::Registry::getSingleton().getTypeName( typeid(**ppt) );
        }

        void getSerializerPolymorphic(const std::string &derivedTypeName) 
        { 
            pf = & SF::Registry::getSingleton().template getSerializerPolymorphic<T>(derivedTypeName);
        }

        bool invokeSerializerPolymorphic(SF::Archive &ar) 
        { 
            SF_ASSERT(pf);
            void **ppvb = (void **) (ppt); // not even reinterpret_cast wants to touch this...
            return pf->invoke(ppvb, ar);
        }

        void serializeContents(Archive &ar) 
        { 
            preserialize(ar, *ppt, 0);
        }

        void addToInputContext(SF::I_Stream *stream, const UInt32 &nid)
        {
            I_ContextRead &ctx = dynamic_cast<I_WithContextRead *>(stream)->getContext();
            ctx.add(nid, IdT( (void *) (*ppt), &typeid(T)));
        }

        bool queryInputContext(SF::I_Stream *stream, const UInt32 &nid)
        {
            I_ContextRead &ctx = dynamic_cast<I_WithContextRead *>(stream)->getContext();
            return ctx.query(nid, id);
        } 

        void addToOutputContext(SF::I_Stream *stream, UInt32 &nid)
        {
            I_ContextWrite &ctx = dynamic_cast<I_WithContextWrite *>(stream)->getContext();
            ctx.add( IdT( (void *) *ppt, &typeid(T)), nid);
        }

        bool queryOutputContext(SF::I_Stream *stream, UInt32 &nid)
        {
            I_ContextWrite &ctx = dynamic_cast<I_WithContextWrite *>(stream)->getContext();
            return ctx.query( IdT( (void *) *ppt, &typeid(T)), nid);
        }

        void setFromId()        
        { 
            *ppt = reinterpret_cast<T *>(id.first); 
        }

        void setToNull()
        { 
            *ppt = NULL; 
        }

        bool isNull() 
        { 
            return *ppt == NULL; 
        }

        bool isNonAtomic()
        {
            bool isFundamental = boost::is_fundamental<T>::value;
            return !isFundamental;
        }

    };

    template<typename T> struct GetIndirection                         { typedef Meta::Int<0> Level; typedef T Base; };
    template<typename T> struct GetIndirection<T *>                    { typedef Meta::Int<1> Level; typedef T Base; };
    template<typename T> struct GetIndirection<T const *>              { typedef Meta::Int<1> Level; typedef T Base; };
    template<typename T> struct GetIndirection<T * const>              { typedef Meta::Int<1> Level; typedef T Base; };
    template<typename T> struct GetIndirection<T const * const>        { typedef Meta::Int<1> Level; typedef T Base; };
    template<typename T> struct GetIndirection<T **>                   { typedef Meta::Int<2> Level; typedef T Base; };
    template<typename T> struct GetIndirection<T **const>              { typedef Meta::Int<2> Level; typedef T Base; };
    template<typename T> struct GetIndirection<T *const*>              { typedef Meta::Int<2> Level; typedef T Base; };
    template<typename T> struct GetIndirection<T const**>              { typedef Meta::Int<2> Level; typedef T Base; };
    template<typename T> struct GetIndirection<T *const*const>         { typedef Meta::Int<2> Level; typedef T Base; };
    template<typename T> struct GetIndirection<T const**const>         { typedef Meta::Int<2> Level; typedef T Base; };
    template<typename T> struct GetIndirection<T const*const*>         { typedef Meta::Int<2> Level; typedef T Base; };
    template<typename T> struct GetIndirection<T const*const*const>    { typedef Meta::Int<2> Level; typedef T Base; };

    template<typename U, typename T, typename TC>
    class InvokeSerializer;

    template<typename U, typename T>
    class InvokeSerializer<U, T, Meta::Int<0> >
    {
    public:
        bool operator()(const U &u, Archive &archive)
        {
            T *pt = const_cast<T *>(&u);
            return Serializer<T>(const_cast<T **>(&pt)).invoke(archive);
        }
    };

    template<typename U, typename T>
    class InvokeSerializer<U, T, Meta::Int<1> >
    {
    public:
        bool operator()(const U &u, Archive &archive)
        {
            return Serializer<T>(const_cast<T **>(&u)).invoke(archive);
        }
    };

    template<typename U, typename T>
    class InvokeSerializer<U, T, Meta::Int<2> >
    {
    public:
        bool operator()(const U &u, Archive &archive)
        {
            return Serializer<T>(const_cast<T **>(u)).invoke(archive);
        }
    };

    template<typename U>
    inline bool invokeSerializer(U u, Archive &ar) 
    {
        typedef typename GetIndirection<U>::Level Level;
        typedef typename GetIndirection<U>::Base T;
        return InvokeSerializer<U, T, Level>()(u, ar);
    }

    template<typename U>
    inline bool invokePtrSerializer(U u, Archive &ar) 
    {
        typedef typename GetIndirection<U>::Level Level;
        const int levelOfIndirection = Level::Val;
        SF_ASSERT( levelOfIndirection == 1 || levelOfIndirection == 2);
        ar.setFlag( SF::Archive::POINTER, levelOfIndirection == 2 );
        return invokeSerializer(u,ar);
    }


    template<typename Archive, typename T>
    inline void preserialize(Archive &ar, T *&pt, const unsigned int version)
    {
        serialize(ar, *pt, version);
    }

    template<typename Archive, typename T>
    inline void serialize(Archive &archive, T &t, const unsigned int version)
    {
        // defer to internal serialization
        t.serialize(archive, version);
    }
    
    template<typename Archive, typename Base, typename Derived>
    class SerializeParent 
    {
    public:
        void operator()(Archive &ar, Derived &derived)
        {
            ar & boost::serialization::base_object<Base>(derived);
        }
    };

    template<typename Base, typename Derived>
    class SerializeParent<SF::Archive, Base, Derived>
    {
    public:
        void operator()(SF::Archive &ar, Derived &derived)
        {
            SF::SerializerPolymorphic<Base,Derived>::instantiate();
            ar & SF::Archive::PARENT & static_cast<Base &>(derived);
        }
    };

}

template<typename Base, typename Archive, typename Derived>
void serializeParent(Archive &ar, Derived &derived)
{
    SF::SerializeParent<Archive, Base, Derived>()(ar, derived);
}

#endif // ! _SF_SERIALIZER_HPP_
