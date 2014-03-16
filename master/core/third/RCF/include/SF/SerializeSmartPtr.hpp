
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_SERIALIZESMARTPTR_HPP_
#define _SF_SERIALIZESMARTPTR_HPP_

#include <SF/Archive.hpp>

namespace SF {

    //********************************************************
    // smart pointer utilities

    template<typename T>
    class Serializer;

    // 1. Non-ref counted smart pointer. SmartPtr<T> must support reset() and operator->().

#ifndef __BORLANDC__

    template< template<typename> class SmartPtr, typename T >
    inline bool serializeSimpleSmartPtr(SmartPtr<T> **pps, SF::Archive &ar)
    {
        bool bRet = false;
        if (ar.isRead()) {
            if (ar.isFlagSet(Archive::POINTER)) *pps = new SmartPtr<T>;
            T *pt = NULL;
            ar & pt;
            (**pps).reset(pt);
        }
        else if (ar.isWrite()) {
            T *pt = NULL;
            if (*pps) pt = (**pps).operator->();
            ar & pt;
        }
        return true;
    }

#define SF_SERIALIZE_SIMPLE_SMARTPTR( SmartPtr )                                        \
    template<typename T>                                                                \
    class Serializer< SmartPtr<T> > {                                                    \
    public:                                                                             \
        SmartPtr<T> **ppt_;                                                                \
        Serializer(SmartPtr<T> **ppt) : ppt_(ppt) {}                                    \
        bool invoke(Archive &ar) { return serializeSimpleSmartPtr(ppt_, ar); }            \
    }; class RequireSemicolon

#else // __BORLANDC__

#define SF_SERIALIZE_SIMPLE_SMARTPTR( SmartPtr )                                        \
    template<typename T>                                                                \
    inline bool serializeSimpleSmartPtr(SmartPtr<T> **pps, SF::Archive &ar)                \
    {                                                                                    \
        bool bRet = false;                                                                \
        if (ar.isRead()) {                                                                \
            if (ar.isFlagSet(Archive::POINTER)) *pps = new SmartPtr<T>;                    \
            T *pt = NULL;                                                                \
            ar & pt;                                                                    \
            (**pps).reset(pt);                                                          \
        }                                                                                \
    else if (ar.isWrite()) {                                                            \
            T *pt = NULL;                                                                \
            if (*pps) pt = (**pps).operator->();                                        \
            ar & pt;                                                                    \
        }                                                                                \
        return true;                                                                    \
    }                                                                                    \
                                                                                        \
    template<typename T>                                                                \
    class Serializer< SmartPtr<T> > {                                                        \
    public:                                                                             \
        SmartPtr<T> **ppt_;                                                                \
        Serializer(SmartPtr<T> **ppt) : ppt_(ppt) {}                                            \
        bool invoke( Archive &ar ) { return serializeSimpleSmartPtr(ppt_, ar); }            \
    }; class RequireSemicolon

#endif

    // 2. Ref counted smart pointer. Must support operator=(), operator->(), and get().

#ifndef __BORLANDC__

    template< template<typename> class SmartPtr, typename T >
    inline bool serializeRefCountedSmartPtr(SmartPtr<T> **pps, SF::Archive &ar)
    {
        SF_ASSERT( ar.isRead() || ar.isWrite() );
        if (ar.isRead()) {
            if (ar.isFlagSet(Archive::POINTER)) *pps = new SmartPtr<T>;
            T *pt = NULL;
            if ((ar & pt).isOk()) {
                typedef ObjectId IdT;
                I_ContextRead &ctx = dynamic_cast<WithContextRead *>(ar.getStream())->getContext();
                void *pv = NULL;
                if (pt && ctx.query( pt, typeid(SmartPtr<T>), pv )) {
                    SmartPtr<T> *ps_prev = reinterpret_cast< SmartPtr<T> * >(pv);
                    **pps = *ps_prev;
                }
                else if (pt) {
                    ctx.add( pt, typeid(SmartPtr<T>), *pps );
                    //**pps = SmartPtr<T>(pt); // causes ICE with codewarrior 9.0
                    SmartPtr<T> spt(pt);
                    **pps = spt;
                }
                else {
                    //**pps = SmartPtr<T>(pt); // causes ICE with codewarrior 9.0
                    SmartPtr<T> spt(pt);
                    **pps = spt;
                }
                return true;
            }
            else
                return false;
        }
        else /*if (ar.isWrite())*/ {
            T *pt = NULL;
            if (*pps) pt = (**pps).get();
            return (ar & pt).isOk();
        }
    }

#define SF_SERIALIZE_REFCOUNTED_SMARTPTR( SmartPtr )                                    \
    template<typename T>                                                                \
    class Serializer< SmartPtr<T> > {                                                        \
    public:                                                                             \
        SmartPtr<T> **ppt_;                                                                \
        Serializer(SmartPtr<T> **ppt) : ppt_(ppt) {}                                            \
        bool invoke( Archive &ar ) { return serializeRefCountedSmartPtr(ppt_, ar); }        \
    }; class RequireSemicolon

#else // __BORLANDC__

#define SF_SERIALIZE_REFCOUNTED_SMARTPTR( SmartPtr )                                                \
    template<typename T >                                                                            \
    inline bool serializeRefCountedSmartPtr(SmartPtr<T> **pps, SF::Archive &ar)                        \
    {                                                                                                \
        if (ar.isRead()) {                                                                            \
            if (ar.isFlagSet(Archive::POINTER)) *pps = new SmartPtr<T>;                                \
            T *pt = NULL;                                                                            \
            if ((ar & pt).isOk()) {                                                                    \
                typedef ObjectId IdT;                                                               \
                I_ContextRead &ctx = dynamic_cast<WithContextRead *>(ar.getStream())->getContext();    \
                void *pv = NULL;                                                                    \
                if (pt && ctx.query( pt, typeid(SmartPtr<T>), pv )) {                                \
                    SmartPtr<T> *ps_prev = reinterpret_cast< SmartPtr<T> * >(pv);                    \
                    **pps = *ps_prev;                                                                \
                }                                                                                    \
                    else if (pt) {                                                                    \
                    ctx.add( pt, typeid(SmartPtr<T>), *pps );                                        \
                    **pps = SmartPtr<T>(pt);                                                        \
                }                                                                                    \
                else {                                                                                \
                    **pps = SmartPtr<T>(pt);                                                        \
                }                                                                                    \
                return true;                                                                        \
            }                                                                                        \
            else                                                                                    \
                return false;                                                                        \
        }                                                                                            \
        else if (ar.isWrite()) {                                                                    \
            T *pt = NULL;                                                                            \
            if (*pps) pt = (**pps).get();                                                            \
            return (ar & pt).isOk();                                                                \
        }                                                                                            \
    }                                                                                                \
                                                                                                    \
    template<typename T>                                                                            \
    class Serializer< SmartPtr<T> > {                                                                    \
    public:                                                                                         \
        SmartPtr<T> **ppt_;                                                                            \
        Serializer(SmartPtr<T> **ppt) : ppt_(ppt) {}                                                        \
        bool invoke( Archive &ar ) { return serializeRefCountedSmartPtr(ppt_, ar); }                    \
    }; class RequireSemicolon

#endif

} // namespace SF

#endif // ! _SF_SERIALIZERSMARTPTR_HPP_
