
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_IDL_HPP_
#define _RCF_IDL_HPP_

#include <boost/shared_ptr.hpp>
#include <RCF/Exception.hpp>
#include <RCF/Marshal.hpp>
#include <RCF/ServerStub.hpp>
#include <RCF/util/Meta.hpp>

// RCF_BEGIN
#define RCF_BEGIN(InterfaceT, Name)                                                                     \
                                                                                                        \
        class InterfaceT {};                                                                            \
                                                                                                        \
        template<typename T>                                                                            \
        class RcfClient;                                                                                \
                                                                                                        \
        inline std::string RCF_typename(InterfaceT *)                                                   \
        {                                                                                               \
            return std::string(Name) == "" ? #InterfaceT : Name;                                        \
        }                                                                                               \
                                                                                                        \
        template<>                                                                                      \
        class RcfClient< InterfaceT > : public ::RCF::ClientStub                                        \
        {                                                                                               \
        public:                                                                                         \
                                                                                                        \
            RcfClient() :                                                                               \
                ::RCF::ClientStub(                                                                      \
                    "",                                                                                 \
                    0,                                                                                  \
                    -1,                                                                                 \
                    false,                                                                              \
                    false,                                                                              \
                    ::RCF::DefaultClientRecvTimeoutMs,                                                  \
                    "",                                                                                 \
                    RCF_typename((InterfaceT *) NULL),                                                  \
                    RCF_typename((InterfaceT *) NULL))                                                  \
                                                                                                        \
            {}                                                                                          \
                                                                                                        \
            RcfClient(const std::string &ip, int port, const std::string &endpointName = "") :          \
                ::RCF::ClientStub(                                                                      \
                    ip,                                                                                 \
                    port,                                                                               \
                    -1,                                                                                 \
                    false,                                                                              \
                    false,                                                                              \
                    ::RCF::DefaultClientRecvTimeoutMs,                                                  \
                    endpointName,                                                                       \
                    RCF_typename((InterfaceT *) NULL),                                                  \
                    RCF_typename((InterfaceT *) NULL))                                                  \
            {}                                                                                          \
                                                                                                        \
        private:                                                                                        \
                                                                                                        \
            template<typename T, typename U>                                                            \
            void invoke(const T &, ::RCF::Connection &, const U &)                                      \
            {                                                                                           \
                throw ::RCF::Exception("dispatch id does not exist");                                   \
            }                                                                                           \
                                                                                                        \
            friend class ::RCF::StubAccess;                                                             \
            typedef RcfClient< InterfaceT > RCF_ThisT;                                                  \
            friend Meta::False RCF_make_next_dispatch_id_func(RCF_ThisT *, ...);
            



// RCF_END
#define RCF_END( InterfaceT )                                                                           \
        };                                                                                              \
                                                                                                        \
        template<typename ImplementationT, typename ImplementationPtrT>                                 \
        inline boost::shared_ptr<RCF::I_ServerStub>                                                     \
        RCF_createServerStub(InterfaceT *, ImplementationT *, ImplementationPtrT px)                    \
        {                                                                                               \
            return boost::shared_ptr<RCF::I_ServerStub>(                                                \
                new ::RCF::ServerStub< RcfClient< InterfaceT >, ImplementationT >(px));                 \
        }

                                                    

// RCF_METHOD_R0
#define RCF_METHOD_R0(R,func)                        _RCF_METHOD_R0(R,func, RCF_MAKE_UNIQUE_ID(func, R0))
#define _RCF_METHOD_R0(R, func, id)                                                                     \
        public:                                                                                         \
            RCF_MAKE_NEXT_DISPATCH_ID(id);                                                              \
            R func()                                                                                    \
            {                                                                                           \
                return ::RCF::ClientMarshal< R >()(*this, id::Val);                                     \
            }                                                                                           \
                                                                                                        \
        private:                                                                                        \
            template<typename T>                                                                        \
            void invoke(const id &, ::RCF::Connection &connection, T &t)                                \
            {                                                                                           \
                ::RCF::IDL::OutReturnValue< R >(connection, t.func());                                  \
            }



// RCF_METHOD_R1
#define RCF_METHOD_R1(R,func,A1)                        _RCF_METHOD_R1(R,func,A1, RCF_MAKE_UNIQUE_ID(func, R1))
#define _RCF_METHOD_R1(R,func,A1,id)                                                                    \
        public:                                                                                         \
            RCF_MAKE_NEXT_DISPATCH_ID(id);                                                              \
            R func(A1 a1)                                                                               \
            {                                                                                           \
                return ::RCF::ClientMarshal< R,A1 >()(*this, id::Val, a1);                              \
            }                                                                                           \
                                                                                                        \
        private:                                                                                        \
            template<typename T>                                                                        \
            void invoke(const id &, ::RCF::Connection &connection, T &t)                                \
            {                                                                                           \
                ::RCF::IDL::InParameter< A1 > arg1(connection);                                         \
                ::RCF::IDL::OutReturnValue< R >(connection, t.func(arg1.get()));                        \
                ::RCF::IDL::OutParameter< A1 >(arg1, connection);                                       \
            }


#define RCF_METHOD_R2(R,func,A1,A2)                        _RCF_METHOD_R2(R,func,A1,A2, RCF_MAKE_UNIQUE_ID(func, R2))
#define _RCF_METHOD_R2(R,func,A1,A2,id)                                                                 \
        public:                                                                                         \
            RCF_MAKE_NEXT_DISPATCH_ID(id);                                                              \
            R func(A1 a1, A2 a2)                                                                        \
            {                                                                                           \
                return ::RCF::ClientMarshal< R,A1,A2 >()(*this, id::Val, a1, a2);                       \
            }                                                                                           \
                                                                                                        \
        private:                                                                                        \
            template<typename T>                                                                        \
            void invoke(const id &, ::RCF::Connection &connection, T &t)                                \
            {                                                                                           \
                ::RCF::IDL::InParameter< A1 > arg1(connection);                                         \
                ::RCF::IDL::InParameter< A2 > arg2(connection);                                         \
                ::RCF::IDL::OutReturnValue< R >(connection, t.func(arg1.get(), arg2.get()));            \
                ::RCF::IDL::OutParameter< A1 >(arg1, connection);                                       \
                ::RCF::IDL::OutParameter< A2 >(arg2, connection);                                       \
            }


// RCF_METHOD_R3
#define RCF_METHOD_R3(R,func,A1,A2,A3)                        _RCF_METHOD_R3(R,func,A1,A2,A3, RCF_MAKE_UNIQUE_ID(func, R3))
#define _RCF_METHOD_R3(R,func,A1,A2,A3,id)                                                              \
        public:                                                                                         \
            RCF_MAKE_NEXT_DISPATCH_ID(id);                                                              \
            R func(A1 a1, A2 a2, A3 a3)                                                                 \
            {                                                                                           \
                return ::RCF::ClientMarshal< R,A1,A2,A3 >()(*this, id::Val, a1, a2, a3);                \
            }                                                                                           \
                                                                                                        \
        private:                                                                                        \
            template<typename T>                                                                        \
            void invoke(const id &, ::RCF::Connection &connection, T &t)                                \
            {                                                                                           \
                ::RCF::IDL::InParameter< A1 > arg1(connection);                                         \
                ::RCF::IDL::InParameter< A2 > arg2(connection);                                         \
                ::RCF::IDL::InParameter< A3 > arg3(connection);                                         \
                ::RCF::IDL::OutReturnValue< R >(connection, t.func(arg1.get(), arg2.get(), arg3.get()));    \
                ::RCF::IDL::OutParameter< A1 >(arg1, connection);                                       \
                ::RCF::IDL::OutParameter< A2 >(arg2, connection);                                       \
                ::RCF::IDL::OutParameter< A3 >(arg3, connection);                                       \
            }

// RCF_METHOD_R4
#define RCF_METHOD_R4(R,func,A1,A2,A3,A4)                        _RCF_METHOD_R4(R,func,A1,A2,A3,A4, RCF_MAKE_UNIQUE_ID(func, R4))
#define _RCF_METHOD_R4(R,func,A1,A2,A3,A4,id)                                                           \
        public:                                                                                         \
            RCF_MAKE_NEXT_DISPATCH_ID(id);                                                              \
            R func(A1 a1, A2 a2, A3 a3, A4 a4 )                                                         \
            {                                                                                           \
                return ::RCF::ClientMarshal< R,A1,A2,A3,A4 >()(*this, id::Val, a1, a2, a3, a4);         \
            }                                                                                           \
                                                                                                        \
        private:                                                                                        \
            template<typename T>                                                                        \
            void invoke(const id &, ::RCF::Connection &connection, T &t)                                \
            {                                                                                           \
                ::RCF::IDL::InParameter< A1 > arg1(connection);                                         \
                ::RCF::IDL::InParameter< A2 > arg2(connection);                                         \
                ::RCF::IDL::InParameter< A3 > arg3(connection);                                         \
                ::RCF::IDL::InParameter< A4 > arg4(connection);                                         \
                ::RCF::IDL::OutReturnValue< R >(connection, t.func(arg1.get(), arg2.get(), arg3.get(), arg4.get()));    \
                ::RCF::IDL::OutParameter< A1 >(arg1, connection);                                       \
                ::RCF::IDL::OutParameter< A2 >(arg2, connection);                                       \
                ::RCF::IDL::OutParameter< A3 >(arg3, connection);                                       \
                ::RCF::IDL::OutParameter< A4 >(arg4, connection);                                       \
            }

// RCF_METHOD_R4
#define RCF_METHOD_R5(R,func,A1,A2,A3,A4,A5)                        _RCF_METHOD_R5(R,func,A1,A2,A3,A4,A5, RCF_MAKE_UNIQUE_ID(func, R5))
#define _RCF_METHOD_R5(R,func,A1,A2,A3,A4,A5, id)                                                       \
        public:                                                                                         \
            RCF_MAKE_NEXT_DISPATCH_ID(id);                                                              \
            R func(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)                                                   \
            {                                                                                           \
                return ::RCF::ClientMarshal< R,A1,A2,A3,A4,A5 >()(*this, id::Val, a1, a2, a3, a4, a5);  \
            }                                                                                           \
                                                                                                        \
        private:                                                                                        \
            template<typename T>                                                                        \
            void invoke(const id &, ::RCF::Connection &connection, T &t)                                \
            {                                                                                           \
            ::RCF::IDL::InParameter< A1 > arg1(connection);                                                \
            ::RCF::IDL::InParameter< A2 > arg2(connection);                                                \
            ::RCF::IDL::InParameter< A3 > arg3(connection);                                                \
            ::RCF::IDL::InParameter< A4 > arg4(connection);                                                \
            ::RCF::IDL::InParameter< A5 > arg5(connection);                                                \
            ::RCF::IDL::OutReturnValue< R >(connection, t.func(arg1.get(), arg2.get(), arg3.get(), arg4.get(), arg5.get()));    \
            ::RCF::IDL::OutParameter< A1 >(arg1, connection);                                           \
            ::RCF::IDL::OutParameter< A2 >(arg2, connection);                                           \
            ::RCF::IDL::OutParameter< A3 >(arg3, connection);                                           \
            ::RCF::IDL::OutParameter< A4 >(arg4, connection);                                           \
            ::RCF::IDL::OutParameter< A5 >(arg5, connection);                                           \
            }


// RCF_METHOD_V0
#define RCF_METHOD_V0(R,func)                        _RCF_METHOD_V0(R,func, RCF_MAKE_UNIQUE_ID(func, V0))
#define _RCF_METHOD_V0(R,func, id)                                                                      \
        public:                                                                                         \
            RCF_MAKE_NEXT_DISPATCH_ID(id);                                                              \
            void func()                                                                                 \
            {                                                                                           \
                ::RCF::ClientMarshal< RCF::Void>()(*this, id::Val);                                     \
            }                                                                                           \
                                                                                                        \
        private:                                                                                        \
            template<typename T>                                                                        \
            void invoke(const id &, ::RCF::Connection &connection, T &t)                                \
            {                                                                                           \
                ::RCF::IDL::OutReturnValue<>(connection, (t.func(), 0));                                \
            }

// RCF_METHOD_V1
#define RCF_METHOD_V1(R,func,A1)                        _RCF_METHOD_V1(R,func,A1, RCF_MAKE_UNIQUE_ID(func, V1))
#define _RCF_METHOD_V1(R,func,A1, id)                                                                   \
        public:                                                                                         \
            RCF_MAKE_NEXT_DISPATCH_ID(id);                                                              \
            void func(A1 a1)                                                                            \
            {                                                                                           \
                ::RCF::ClientMarshal< RCF::Void, A1 >()(*this, id::Val, a1);                            \
            }                                                                                           \
                                                                                                        \
        private:                                                                                        \
            template<typename T>                                                                        \
            void invoke(const id &, ::RCF::Connection &connection, T &t)                                \
            {                                                                                           \
                ::RCF::IDL::InParameter< A1 > arg1(connection);                                         \
                ::RCF::IDL::OutReturnValue<>(connection, (t.func(arg1.get()), 0));                      \
                ::RCF::IDL::OutParameter< A1 >(arg1, connection);                                       \
            }


// RCF_METHOD_V2
#define RCF_METHOD_V2(R,func,A1,A2)                        _RCF_METHOD_V2(R,func,A1,A2, RCF_MAKE_UNIQUE_ID(func, V2))
#define _RCF_METHOD_V2(R,func,A1,A2, id)                                                                \
        public:                                                                                         \
            RCF_MAKE_NEXT_DISPATCH_ID(id);                                                              \
            void func(A1 a1, A2 a2)                                                                     \
            {                                                                                           \
                ::RCF::ClientMarshal< RCF::Void, A1, A2 >()(*this, id::Val, a1, a2);                    \
            }                                                                                           \
                                                                                                        \
        private:                                                                                        \
            template<typename T>                                                                        \
            void invoke(const id &, ::RCF::Connection &connection, T &t)                                \
            {                                                                                           \
                ::RCF::IDL::InParameter< A1 > arg1(connection);                                         \
                ::RCF::IDL::InParameter< A2 > arg2(connection);                                         \
                ::RCF::IDL::OutReturnValue<>(connection, (t.func(arg1.get(), arg2.get()), 0));          \
                ::RCF::IDL::OutParameter< A1 >(arg1, connection);                                       \
                ::RCF::IDL::OutParameter< A2 >(arg2, connection);                                       \
            }


// RCF_METHOD_V3
#define RCF_METHOD_V3(R,func,A1,A2,A3)                        _RCF_METHOD_V3(R,func,A1,A2,A3, RCF_MAKE_UNIQUE_ID(func, V3))
#define _RCF_METHOD_V3(R,func,A1,A2,A3, id)                                                             \
        public:                                                                                         \
            RCF_MAKE_NEXT_DISPATCH_ID(id);                                                              \
            void func(A1 a1, A2 a2, A3 a3)                                                              \
            {                                                                                           \
                ::RCF::ClientMarshal< RCF::Void, A1, A2, A3 >()(*this, id::Val, a1, a2, a3);            \
            }                                                                                           \
                                                                                                        \
        private:                                                                                        \
            template<typename T>                                                                        \
            void invoke(const id &, ::RCF::Connection &connection, T &t)                                \
            {                                                                                           \
                ::RCF::IDL::InParameter< A1 > arg1(connection);                                         \
                ::RCF::IDL::InParameter< A2 > arg2(connection);                                         \
                ::RCF::IDL::InParameter< A3 > arg3(connection);                                         \
                ::RCF::IDL::OutReturnValue<>(connection, (t.func(arg1.get(), arg2.get(), arg3.get()), 0));    \
                ::RCF::IDL::OutParameter< A1 >(arg1, connection);                                       \
                ::RCF::IDL::OutParameter< A2 >(arg2, connection);                                       \
                ::RCF::IDL::OutParameter< A3 >(arg3, connection);                                       \
            }
        
// RCF_METHOD_V4
#define RCF_METHOD_V4(R,func,A1,A2,A3,A4)                        _RCF_METHOD_V4(R,func,A1,A2,A3,A4, RCF_MAKE_UNIQUE_ID(func, V4))
#define _RCF_METHOD_V4(R,func,A1,A2,A3,A4, id)                                                          \
        public:                                                                                         \
            RCF_MAKE_NEXT_DISPATCH_ID(id);                                                              \
            void func(A1 a1, A2 a2, A3 a3, A4 a4)                                                       \
            {                                                                                           \
                ::RCF::ClientMarshal< RCF::Void, A1, A2, A3, A4 >()(*this, id::Val, a1, a2, a3, a4);    \
            }                                                                                           \
                                                                                                        \
        private:                                                                                        \
            template<typename T>                                                                        \
            void invoke(const id &, ::RCF::Connection &connection, T &t)                                \
            {                                                                                           \
                ::RCF::IDL::InParameter< A1 > arg1(connection);                                         \
                ::RCF::IDL::InParameter< A2 > arg2(connection);                                         \
                ::RCF::IDL::InParameter< A3 > arg3(connection);                                         \
                ::RCF::IDL::InParameter< A4 > arg4(connection);                                         \
                ::RCF::IDL::OutReturnValue<>(connection, (t.func(arg1.get(), arg2.get(), arg3.get(), arg4.get()), 0));    \
                ::RCF::IDL::OutParameter< A1 >(arg1, connection);                                       \
                ::RCF::IDL::OutParameter< A2 >(arg2, connection);                                       \
                ::RCF::IDL::OutParameter< A3 >(arg3, connection);                                       \
                ::RCF::IDL::OutParameter< A4 >(arg4, connection);                                       \
            }

// RCF_METHOD_V5
#define RCF_METHOD_V5(R,func,A1,A2,A3,A4,A5)                        _RCF_METHOD_V5(R,func,A1,A2,A3,A4,A5, RCF_MAKE_UNIQUE_ID(func, V5))
#define _RCF_METHOD_V5(R,func,A1,A2,A3,A4,A5, id)                                                       \
        public:                                                                                         \
            RCF_MAKE_NEXT_DISPATCH_ID(id);                                                              \
            void func(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)                                                \
            {                                                                                           \
                ::RCF::ClientMarshal< RCF::Void, A1, A2, A3, A4, A5 >()(*this, id::Val, a1, a2, a3, a4, a5);    \
            }                                                                                           \
                                                                                                        \
        private:                                                                                        \
            template<typename T>                                                                        \
            void invoke(const id &, ::RCF::Connection &connection, T &t)                                \
            {                                                                                           \
                ::RCF::IDL::InParameter< A1 > arg1(connection);                                         \
                ::RCF::IDL::InParameter< A2 > arg2(connection);                                         \
                ::RCF::IDL::InParameter< A3 > arg3(connection);                                         \
                ::RCF::IDL::InParameter< A4 > arg4(connection);                                         \
                ::RCF::IDL::InParameter< A5 > arg5(connection);                                         \
                ::RCF::IDL::OutReturnValue<>(connection, (t.func(arg1.get(), arg2.get(), arg3.get(), arg4.get(), arg5.get()), 0));    \
                ::RCF::IDL::OutParameter< A1 >(arg1, connection);                                       \
                ::RCF::IDL::OutParameter< A2 >(arg2, connection);                                       \
                ::RCF::IDL::OutParameter< A3 >(arg3, connection);                                       \
                ::RCF::IDL::OutParameter< A4 >(arg4, connection);                                       \
                ::RCF::IDL::OutParameter< A5 >(arg5, connection);                                       \
            }


// RCF_MAKE_UNIQUE_ID
#define RCF_MAKE_UNIQUE_ID(func, sig)                        _RCF_MAKE_UNIQUE_ID(func, sig, __LINE__)
#define _RCF_MAKE_UNIQUE_ID(func, sig, __LINE__)            __RCF_MAKE_UNIQUE_ID(func, sig, __LINE__)
#define __RCF_MAKE_UNIQUE_ID(func, sig, Line)                rcf_unique_id_##func##_##sig##_##Line

// RCF_MAKE_NEXT_DISPATCH_ID
#define RCF_MAKE_NEXT_DISPATCH_ID(next_dispatch_id)                                                                                                 \
    typedef Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int< 0> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int< 1> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int< 2> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int< 3> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int< 4> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int< 5> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int< 6> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int< 7> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int< 8> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int< 9> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<10> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<11> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<12> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<13> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<14> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<15> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<16> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<17> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<18> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<19> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<20> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<21> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<22> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<23> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Select< Meta::Bool< sizeof(RCF_make_next_dispatch_id_func((RCF_ThisT *) 0, (Meta::Int<24> *) 0)) == sizeof(Meta::True) >::Val,    \
            Meta::Int<25>,                            \
            Meta::Int<24> >::Val,                   \
            Meta::Int<23> >::Val,                   \
            Meta::Int<22> >::Val,                   \
            Meta::Int<21> >::Val,                   \
            Meta::Int<20> >::Val,                   \
            Meta::Int<19> >::Val,                   \
            Meta::Int<18> >::Val,                   \
            Meta::Int<17> >::Val,                   \
            Meta::Int<16> >::Val,                   \
            Meta::Int<15> >::Val,                   \
            Meta::Int<14> >::Val,                   \
            Meta::Int<13> >::Val,                   \
            Meta::Int<12> >::Val,                   \
            Meta::Int<11> >::Val,                   \
            Meta::Int<10> >::Val,                   \
            Meta::Int< 9> >::Val,                   \
            Meta::Int< 8> >::Val,                   \
            Meta::Int< 7> >::Val,                   \
            Meta::Int< 6> >::Val,                   \
            Meta::Int< 5> >::Val,                   \
            Meta::Int< 4> >::Val,                   \
            Meta::Int< 3> >::Val,                   \
            Meta::Int< 2> >::Val,                   \
            Meta::Int< 1> >::Val,                   \
            Meta::Int< 0> >::Val next_dispatch_id;  \
    friend Meta::True RCF_make_next_dispatch_id_func(RCF_ThisT *, next_dispatch_id *)

#endif // ! _RCF_IDL_HPP_
