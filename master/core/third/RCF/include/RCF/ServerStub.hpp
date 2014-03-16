
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_SERVERSTUB_HPP_
#define _RCF_SERVERSTUB_HPP_

#include <RCF/Connection.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>

#include <RCF/Token.hpp>

namespace RCF {

    class Connection;

    class I_ServerStub
    {
    public:
        virtual ~I_ServerStub() {}
        virtual void invoke(int id, Connection &connection) = 0;
        virtual void setToken(Token token) = 0;
        virtual Token getToken() = 0;
    };
    
    template<typename StubT, typename ImplementationT>
    class ServerStub : public I_ServerStub, boost::noncopyable
    {
    public:
        ServerStub(ImplementationT &x);
        ServerStub(std::auto_ptr<ImplementationT> px);
        ServerStub(boost::shared_ptr<ImplementationT> px);

    private:
        void invoke(int id, RCF::Connection &connection);
        template<typename IdT> void invoke(StubT &stub, const IdT &id, Connection &connection, ImplementationT &t);
        Token getToken();
        void setToken(Token token);
    
        StubT stub;
        Token token;
        boost::shared_ptr<ImplementationT> px;
        ImplementationT &x;
    };

    class StubAccess
    {
    public:
        template<typename StubT, typename IdT, typename T>
        void invoke(StubT &stub, const IdT &id, Connection &connection, T &t);
    };

} // namespace RCF

#include <RCF/ServerStub.inl>

#endif // ! _RCF_SERVERSTUB_HPP_
