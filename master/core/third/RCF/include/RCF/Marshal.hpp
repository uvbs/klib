
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_MARSHAL_HPP_
#define _RCF_MARSHAL_HPP_

#include <RCF/ClientStub.hpp>
#include <RCF/Connection.hpp>
#include <RCF/MethodInvocation.hpp>
#include <RCF/ClientStub.hpp>
#include <RCF/Tools.hpp>
#include <RCF/util/Meta.hpp>

#include <SF/Tools.hpp>

#include <boost/mpl/not.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

namespace RCF {

    // Boost.Serialization treats pointers of primitive types differently than other types,
    // hence the following hack, which unfortunately overrides all the serialization protocols,
    // not just Boost.Serialization.

/*#define RCF_SERIALIZE_PRIMITIVE_POINTER(type)                                                                                \*/
#define DEFINE_RCF_POINTER_SERIALIZATION(type)                                                                                \
    inline Connection &operator<<(Connection &connection, const type *pt) { return connection << *pt; }                        \
    inline Connection &operator<<(Connection &connection, type *const pt) { return connection << *pt; }                        \
    inline Connection &operator>>(Connection &connection, type *&pt) { RCF_ASSERT(pt==NULL); pt = new type(); return connection >> *pt; }

    FOR_EACH_FUNDAMENTAL_TYPE( DEFINE_RCF_POINTER_SERIALIZATION )

	DEFINE_RCF_POINTER_SERIALIZATION(std::string);

#undef DEFINE_RCF_POINTER_SERIALIZATION

    // Boost.Serialization handles smart pointers very clumsily (up to v. 1.32.0 at least), so we'll do those ourselves

    //#define RCF_SERIALIZE_REFCOUNTSMARTPTR(RefCountSmartPtr)                                            

#define RefCountSmartPtr boost::shared_ptr

    template<typename T>
    inline Connection &operator<<(Connection &connection, const RefCountSmartPtr<T> *spt)
    {
        return connection << *spt;
    }

    template<typename T>
    inline Connection &operator<<(Connection &connection, RefCountSmartPtr<T> *const spt)
    {
        return connection << *spt;
    }

    template<typename T>
    inline Connection &operator>>(Connection &connection, RefCountSmartPtr<T> *&spt)
    {
        spt = new RefCountSmartPtr<T>();
        return connection >> *spt;
    }

    template<typename T>
    inline Connection &operator<<(Connection &connection, const RefCountSmartPtr<T> &spt)
    {
        return connection << spt.get();
    }

    template<typename T>
    inline Connection &operator>>(Connection &connection, RefCountSmartPtr<T> &spt)
    {
        T *pt = NULL;
        connection >> pt;
        RefCountSmartPtr<T> *pspt = 
            connection.getPointerContext().template get< RefCountSmartPtr<T> * >(pt);
        if (pspt == NULL)
        {
            spt = RefCountSmartPtr<T>(pt);
            connection.getPointerContext().template set< RefCountSmartPtr<T> * >(&spt, pt);
        }
        else
        {
            spt = *pspt;
        }
        return connection;
    }

#undef RefCountSmartPtr



    struct Void {};

    namespace IDL {

        class DoRequest 
        {
        public:
            DoRequest(ClientStub &stub, int fnId, Connection &connection)
            {
                connection.setProtocol(stub.getProtocol());
                connection.prepareSend();
                connection << RCF::MethodInvocationRequest(stub.getToken(), stub.getObjectName(), fnId, stub.getOneway(), false);
            }
        };

        template<typename T>
        class InParameter
        {
        public:
            BOOST_STATIC_ASSERT( boost::mpl::not_< typename boost::is_pointer<T>::type >::value );
            InParameter(const T &t, Connection &connection) : t_() { connection << t; }
            InParameter(Connection &connection) : t_() { connection >> t_; }
            const T &get() { return t_; }
        private:
            T t_;
        };

        template<typename T>
        class InParameter<T *>
        {
        public:
            typedef typename boost::remove_const<T>::type U;
            InParameter(T *pt, Connection &connection) { connection << pt; }
            InParameter(Connection &connection)
            {
                U *pu = NULL;
                connection >> pu;
                boost::shared_ptr<U> *ppu = connection.getPointerContext().template get< boost::shared_ptr<U>* >(pu);
                if (ppu == NULL)
                {
                    pu_ = boost::shared_ptr<U>(pu);
                    connection.getPointerContext().template set< boost::shared_ptr<U>* >(&pu_, pu);
                }
                else
                {
                    pu_ = *ppu;
                }
            }
            T *get() { return pu_.get(); }
        private:
            boost::shared_ptr<U> pu_;
        };

        template<typename T>
        class InParameter<T &>
        {
        public:
            typedef typename boost::remove_const<T>::type U;
            InParameter(T &t, Connection &connection) { connection << &t; }
            InParameter(Connection &connection) 
            {
                U *pu = NULL;
                connection >> pu;
                boost::shared_ptr<U> *ppu = connection.getPointerContext().template get< boost::shared_ptr<U>* >(pu);
                if (ppu == NULL)
                {
                    pu_ = boost::shared_ptr<U>(pu);
                    connection.getPointerContext().template set< boost::shared_ptr<U>* >(&pu_, pu);
                }
                else
                {
                    pu_ = *ppu;
                }
            }
            T &get() { return *pu_; }
        private:
            boost::shared_ptr<U> pu_;
        };

        template<typename T>
        class InParameter<T*&>
        {
        public:
            typedef typename boost::remove_const<T>::type U;
            InParameter(U *&pt, Connection &connection) {}
            InParameter(Connection &connection) : pt() {}
            T *&get() { return pt; }
        private:
            T *pt;
        };



        template<typename T>
        class OutParameter
        {
        public:
            OutParameter(const T &t, Connection &connection, bool oneway) {}
            template<typename V> OutParameter( InParameter< V > &v, Connection &connection) {}
        };

        // TODO: replace V with InParameter!

        template<typename T>
        class OutParameter<const T &>
        {
        public:
            OutParameter(const T &t, Connection &connection, bool oneway) {}
            template<typename V> OutParameter( InParameter< V > &v, Connection &connection) {}
        };

        template<typename T>
        class OutParameter<T &>
        {
        public:
            OutParameter(T &t, Connection &connection, bool oneway) { connection >> t; }
            template<typename V> OutParameter( InParameter< V > &v, Connection &connection) { connection << v.get(); }
        };

        template<typename T>
        class OutParameter<const T *&>
        {
        public:
            OutParameter(const T *&pt, Connection &connection, bool oneway) {}
            template<typename V> OutParameter( InParameter< V > &v, Connection &connection) {}
        };

        template<typename T>
        class OutParameter<T *&>
        {
        public:
            OutParameter(T *&pt, Connection &connection, bool oneway) { connection >> pt; }
            template<typename V> OutParameter( InParameter< V > &v, Connection &connection) { connection << v.get(); }
        };

        template<typename T = Void>
        class OutReturnValue 
        {
        public:
            OutReturnValue(Connection &connection, const T &t)
            {
                connection.prepareSend();
                connection << RCF::MethodInvocationResponse(false); 
                connection << t;
            }
        };

        template<>
        class OutReturnValue<Void>
        {
        public:
            OutReturnValue(Connection &connection, int t)
            {
                connection.prepareSend();
                connection << RCF::MethodInvocationResponse(false); 
            }
        };

        template<typename T = Void>
        class InReturnValue 
        {
        public:
            BOOST_STATIC_ASSERT( boost::mpl::not_< typename boost::is_pointer<T>::type >::value );
            BOOST_STATIC_ASSERT( boost::mpl::not_< typename boost::is_reference<T>::type >::value );
            InReturnValue(Connection &connection, bool oneway);
            T &get();
        private:
            T t_;
        };

        template<>
        class InReturnValue<Void> 
        {
        public:
            InReturnValue(Connection &connection, bool oneway);
            Void get() { return Void(); }
        };

    } // namespace IDL


    // ClientMarshal

    template<
        typename R, 
        typename A1 = Meta::Null, 
        typename A2 = Meta::Null, 
        typename A3 = Meta::Null, 
        typename A4 = Meta::Null,
        typename A5 = Meta::Null,
        typename A6 = Meta::Null>
    class ClientMarshal;

    template<typename R>
    class ClientMarshal<R, Meta::Null, Meta::Null, Meta::Null, Meta::Null, Meta::Null, Meta::Null>
    {
    public:
        R operator()(ClientStub &clientStub, int fnId) const
        {
            ScopeGuard connectionResetGuard = MakeObjGuard(clientStub, &ClientStub::releaseConnection);
            bool oneway = clientStub.getOneway();
            RCF::Connection &connection = clientStub.getConnection();
            RCF::IDL::DoRequest(clientStub, fnId, connection);
            RCF::IDL::InReturnValue< R > ret(connection, oneway);
            connectionResetGuard.Dismiss();
            return ret.get();
        }
    };

    template<typename R, typename A1>
    class ClientMarshal<R,A1, Meta::Null, Meta::Null, Meta::Null, Meta::Null, Meta::Null>
    {
    public:
        R operator()(ClientStub &clientStub, int fnId, A1 a1) const
        {
            ScopeGuard connectionResetGuard = MakeObjGuard(clientStub, &ClientStub::releaseConnection);
            bool oneway = clientStub.getOneway();
            RCF::Connection &connection = clientStub.getConnection();
            RCF::IDL::DoRequest(clientStub, fnId, connection);
            RCF::IDL::InParameter< A1 >(a1, connection);
            RCF::IDL::InReturnValue< R > ret(connection, oneway);
            RCF::IDL::OutParameter< A1 >(a1, connection, oneway);
            connectionResetGuard.Dismiss();
            return ret.get();
        }
    };

    template<typename R, typename A1, typename A2>
    class ClientMarshal<R,A1,A2, Meta::Null, Meta::Null, Meta::Null, Meta::Null >
    {
    public:
        R operator()(ClientStub &clientStub, int fnId, A1 a1, A2 a2) const
        {
            ScopeGuard connectionResetGuard = MakeObjGuard(clientStub, &ClientStub::releaseConnection);
            bool oneway = clientStub.getOneway();
            RCF::Connection &connection = clientStub.getConnection();
            RCF::IDL::DoRequest(clientStub, fnId, connection);
            RCF::IDL::InParameter< A1 >(a1, connection);
            RCF::IDL::InParameter< A2 >(a2, connection);
            RCF::IDL::InReturnValue< R > ret(connection, oneway);
            RCF::IDL::OutParameter< A1 >(a1, connection, oneway);
            RCF::IDL::OutParameter< A2 >(a2, connection, oneway);
            connectionResetGuard.Dismiss();
            return ret.get();
        }
    };

    template<typename R, typename A1, typename A2, typename A3>
    class ClientMarshal<R,A1,A2,A3, Meta::Null, Meta::Null, Meta::Null>
    {
    public:
        R operator()(ClientStub &clientStub, int fnId, A1 a1, A2 a2, A3 a3) const
        {
            ScopeGuard connectionResetGuard = MakeObjGuard(clientStub, &ClientStub::releaseConnection);
            bool oneway = clientStub.getOneway();
            RCF::Connection &connection = clientStub.getConnection();
            RCF::IDL::DoRequest(clientStub, fnId, connection);
            RCF::IDL::InParameter< A1 >(a1, connection);
            RCF::IDL::InParameter< A2 >(a2, connection);
            RCF::IDL::InParameter< A3 >(a3, connection);
            RCF::IDL::InReturnValue< R > ret(connection, oneway);
            RCF::IDL::OutParameter< A1 >(a1, connection, oneway);
            RCF::IDL::OutParameter< A2 >(a2, connection, oneway);
            RCF::IDL::OutParameter< A3 >(a3, connection, oneway);
            connectionResetGuard.Dismiss();
            return ret.get();
        }
    };

    template<typename R, typename A1, typename A2, typename A3, typename A4>
    class ClientMarshal<R,A1,A2,A3,A4, Meta::Null, Meta::Null>
    {
    public:
        R operator()(ClientStub &clientStub, int fnId, A1 a1, A2 a2, A3 a3, A4 a4) const
        {
            ScopeGuard connectionResetGuard = MakeObjGuard(clientStub, &ClientStub::releaseConnection);
            bool oneway = clientStub.getOneway();
            RCF::Connection &connection = clientStub.getConnection();
            RCF::IDL::DoRequest(clientStub, fnId, connection);
            RCF::IDL::InParameter< A1 >(a1, connection);
            RCF::IDL::InParameter< A2 >(a2, connection);
            RCF::IDL::InParameter< A3 >(a3, connection);
            RCF::IDL::InParameter< A4 >(a4, connection);
            RCF::IDL::InReturnValue< R > ret(connection, oneway);
            RCF::IDL::OutParameter< A1 >(a1, connection, oneway);
            RCF::IDL::OutParameter< A2 >(a2, connection, oneway);
            RCF::IDL::OutParameter< A3 >(a3, connection, oneway);
            RCF::IDL::OutParameter< A4 >(a4, connection, oneway);
            connectionResetGuard.Dismiss();
            return ret.get();
        }
    };

    template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
    class ClientMarshal<R,A1,A2,A3,A4,A5, Meta::Null>
    {
    public:
        R operator()(ClientStub &clientStub, int fnId, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
        {
            ScopeGuard connectionResetGuard = MakeObjGuard(clientStub, &ClientStub::releaseConnection);
            bool oneway = clientStub.getOneway();
            RCF::Connection &connection = clientStub.getConnection();
            RCF::IDL::DoRequest(clientStub, fnId, connection);
            RCF::IDL::InParameter< A1 >(a1, connection);
            RCF::IDL::InParameter< A2 >(a2, connection);
            RCF::IDL::InParameter< A3 >(a3, connection);
            RCF::IDL::InParameter< A4 >(a4, connection);
            RCF::IDL::InParameter< A5 >(a5, connection);
            RCF::IDL::InReturnValue< R > ret(connection, oneway);
            RCF::IDL::OutParameter< A1 >(a1, connection, oneway);
            RCF::IDL::OutParameter< A2 >(a2, connection, oneway);
            RCF::IDL::OutParameter< A3 >(a3, connection, oneway);
            RCF::IDL::OutParameter< A4 >(a4, connection, oneway);
            RCF::IDL::OutParameter< A5 >(a5, connection, oneway);
            connectionResetGuard.Dismiss();
            return ret.get();
        }
    };

} // namespace RCF

#include <RCF/Marshal.inl>

#endif // ! _RCF_MARSHAL_HPP_
