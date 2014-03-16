
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_REGISTRY_HPP_
#define _SF_REGISTRY_HPP_

#include <map>
#include <string>
#include <typeinfo>

#include <boost/shared_ptr.hpp>

#include <RCF/ThreadLibrary.hpp>
//#include <RCF/util/AutoRun.hpp>
#include <RCF/util/InitDeinit.hpp>

#include <SF/Exception.hpp>
#include <SF/SerializePolymorphic.hpp>
#include <SF/Tools.hpp>

namespace SF {

    class Registry : boost::noncopyable
    {
    private:
        Registry() : readWriteMutex(Platform::Threads::writer_priority) {}
        //typedef const std::type_info * Rtti; // not safe, several pointers might correspond to single type
        typedef std::string Rtti;
        std::map<std::string, Rtti> typeRttis;
        std::map<Rtti, std::string> typeNames;
        std::map<std::pair<Rtti, Rtti>, boost::shared_ptr<I_SerializerPolymorphic> > serializerPolymorphicInstances;

        typedef Platform::Threads::read_write_mutex ReadWriteMutex;
        typedef Platform::Threads::read_write_mutex::scoped_read_lock ReadLock;
        typedef Platform::Threads::read_write_mutex::scoped_write_lock WriteLock;

        ReadWriteMutex readWriteMutex;

    public:

        static Registry &getSingleton()
        {
            static Registry registry;
            return registry;
        }

        static Registry *getSingletonPtr()
        {
            return &getSingleton();
        }

        template<typename Type>
        void registerType(const std::string &typeName)
        {
            WriteLock lock(readWriteMutex); RCF_UNUSED_VARIABLE(lock);
            Rtti typeRtti = typeid(Type).name();
            typeNames[typeRtti] = typeName;
            typeRttis[typeName] = typeRtti;

            // instantiate Type's serializer function so we can register the base/derived info
            // TODO: release build optimizers had better not eliminate this.
            if (0)
            {
                serialize( *((Archive *) NULL), *((Type *) NULL), 0);
            }
        }

        template<typename Base, typename Derived>
        void registerBaseAndDerived()
        {
            WriteLock lock(readWriteMutex); RCF_UNUSED_VARIABLE(lock);
            Rtti baseRtti = typeid(Base).name();
            Rtti derivedRtti = typeid(Derived).name();
            std::pair<Rtti, Rtti> baseDerivedRtti(baseRtti, derivedRtti);
            serializerPolymorphicInstances[baseDerivedRtti].reset(
                new SerializerPolymorphic<Base,Derived>);
        }

        template<typename Base>
        I_SerializerPolymorphic &getSerializerPolymorphic(const std::string &derivedTypeName)
        {
            ReadLock lock(readWriteMutex); RCF_UNUSED_VARIABLE(lock);
            Rtti baseRtti = typeid(Base).name();
            Rtti derivedRtti = typeRttis[derivedTypeName];
            std::pair<Rtti, Rtti> baseDerivedRtti(baseRtti, derivedRtti);
            if (serializerPolymorphicInstances.find(baseDerivedRtti) == serializerPolymorphicInstances.end())
            {
                SF_THROW(SF::Exception, "base/derived pair not registered for types")(derivedTypeName)(baseRtti)(derivedRtti);
            }
            return *serializerPolymorphicInstances[ std::make_pair(baseRtti, derivedRtti) ];
        }

        bool isTypeRegistered(const std::string &typeName)
        {
            ReadLock lock(readWriteMutex); RCF_UNUSED_VARIABLE(lock);
            return typeRttis.find(typeName) != typeRttis.end();
        }

        bool isTypeRegistered(const std::type_info &ti)
        {
            ReadLock lock(readWriteMutex); RCF_UNUSED_VARIABLE(lock);
            Rtti typeRtti = ti.name();
            return typeNames.find(typeRtti) != typeNames.end();
        }

        template<typename T>
        std::string getTypeName()
        {
            return getTypeName(typeid(T));
        }

        std::string getTypeName(const std::type_info &ti)
        {
            ReadLock lock(readWriteMutex); RCF_UNUSED_VARIABLE(lock);
            Rtti typeRtti = ti.name();
            if (typeNames.find(typeRtti) == typeNames.end())
            {
                return "";
            }
            else
            {
                return typeNames[typeRtti];
            }
        }

    };

    UTIL_ON_INIT( Registry::getSingleton() )

    template<typename Derived>
    inline void registerType(const std::string &typeName)
    {
        Registry::getSingleton().template registerType<Derived>(typeName);
    }

    template<typename Base, typename Derived>
    inline void registerBaseAndDerived()
    {
        Registry::getSingleton().template registerBaseAndDerived<Base,Derived>();
    }

} // namespace SF

#endif // ! _SF_REGISTRY_HPP_
