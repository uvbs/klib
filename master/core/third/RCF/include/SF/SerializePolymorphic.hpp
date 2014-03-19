
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_SERIALIZEPOLYMORPHIC_HPP_
#define _SF_SERIALIZEPOLYMORPHIC_HPP_

namespace SF {

    class Archive;

    class I_SerializerPolymorphic
    {
    public:
        virtual ~I_SerializerPolymorphic() {}
        virtual bool invoke(void **ppvb, Archive &ar) = 0;
    };

    template<typename Base, typename Derived>
    void registerBaseAndDerived();

    template<typename Base, typename Derived>
    class SerializerPolymorphic : public I_SerializerPolymorphic
    {
    public:
        static SerializerPolymorphic &instantiate() { return instance; }
        SerializerPolymorphic() {}
        bool invoke(void **ppvb, Archive &ar);

    private:
        static SerializerPolymorphic instance;
        SerializerPolymorphic(int) 
        { 
            SF::registerBaseAndDerived<Base, Derived>(); 
        }
    };

    template<class Base, class Derived>
    SerializerPolymorphic<Base, Derived> SerializerPolymorphic<Base, Derived>::instance(0);

}

#include <SF/Archive.hpp>

//SF::Archive ar(SF::Archive::WRITE, NULL);

namespace SF {

    template<typename Base, typename Derived>
    bool SerializerPolymorphic<Base,Derived>::invoke(void **ppvb, Archive &ar)
    {
        if (ar.isWrite()) 
        {
            Base *pb = reinterpret_cast<Base *>(*ppvb);
            Derived *pd = dynamic_cast<Derived *>(pb);
            ar & pd;
        }
        else if (ar.isRead()) 
        {
            if (ar.isFlagSet(Archive::POINTER)) 
            {
                Derived *pd = NULL;
                ar & pd;
                Base *pb = static_cast<Base *>(pd);
                *ppvb = pb;
            }
            else 
            {
                Base *pb = reinterpret_cast<Base *>(*ppvb);
                Derived *pd = dynamic_cast<Derived *>(pb);
                ar & *pd;
            }
        }
        return ar.isOk();
    }

}

#endif // ! _SF_SERIALIZEPOLYMORPHIC_HPP_
