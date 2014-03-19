
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#ifndef _RCF_RCFSERVER_INL_
#define _RCF_RCFSERVER_INL_

#include <RCF/RcfServer.hpp>

#include <boost/bind.hpp>
#include <boost/ref.hpp>

namespace RCF {

    //****************************************
    // StubFactory

    template<typename T, typename I_T>
    boost::shared_ptr<I_ServerStub> StubFactory<T, I_T>::makeServerStub()
    {
        RCF_TRACE("")(typeid(T))(typeid(I_T));
        std::auto_ptr<T> px( new T );
        return RCF_createServerStub( (I_T *) NULL, (T *) NULL, px ); 
    }

    template<typename T, typename I_T>
    std::string StubFactory<T, I_T>::getObjectName() 
    { 
        RCF_ASSERT(0); 
        return ""; 
    }

    //*********************************
    // SharedStubFactory

    template<typename T, typename I_T>
    boost::shared_ptr<I_ServerStub> SharedStubFactory<T, I_T>::makeServerStub()
    {
        RCF_ASSERT(0);
        return boost::shared_ptr<I_ServerStub>();
    }

    template<typename T, typename I_T>
    boost::shared_ptr<I_ServerStub> SharedStubFactory<T, I_T>::makeServerStub(T &x)
    {
        return RCF_createServerStub( (I_T *) NULL, (T *) NULL, boost::ref(x) ); 
    }

    template<typename T, typename I_T>
    boost::shared_ptr<I_ServerStub> SharedStubFactory<T, I_T>::makeServerStub(std::auto_ptr<T> px)
    {
        RCF_ASSERT(px.get());
        return RCF_createServerStub( (I_T *) NULL, (T *) NULL, px ); 
    }

    template<typename T, typename I_T>
    boost::shared_ptr<I_ServerStub> SharedStubFactory<T, I_T>::makeServerStub(boost::shared_ptr<T> px)
    {
        RCF_ASSERT(px.get());
        return RCF_createServerStub( (I_T *) NULL, (T *) NULL, px ); 
    }

    template<typename T, typename I_T>
    std::string SharedStubFactory<T, I_T>::getObjectName() 
    { 
        RCF_ASSERT(0); 
        return ""; 
    }

    //********************************************
    // ObjectFactory

    template<typename ServerT>
    ObjectFactory<ServerT>::ObjectFactory(ServerT &server) 
        : server_(server) 
    {}
    
    template<typename ServerT>
    Token ObjectFactory<ServerT>::make(const std::string &objectName) 
    { 
        return server_.createClientStub(objectName);
    }

    //********************************************
    // RcfServer

    template<typename InterfaceT, typename ImplementationT> 
    void RcfServer::bind(bool share, std::string name)
    {
        RCF_ASSERT(!running_);

        if (name == "")
        {
            name = RCF_typename((InterfaceT *) NULL);
        }

        if (share)
        {
            bind<InterfaceT>(
                std::auto_ptr<ImplementationT>( new ImplementationT ),
                name);
        }
        else
        {
            registerStubFactory(
                name,
                "",
                boost::shared_ptr<I_StubFactory>( new RCF::StubFactory<ImplementationT, InterfaceT>() ));
        }
        
    }

    template<typename InterfaceT, typename ImplementationT> 
    void RcfServer::bind(ImplementationT &x, std::string name)
    {
        bindShared<InterfaceT, ImplementationT>(
            name, 
            RCF::SharedStubFactory<ImplementationT, InterfaceT>().makeServerStub(x));
    }

    template<typename InterfaceT, typename ImplementationT> 
    void RcfServer::bind(std::auto_ptr<ImplementationT> px, std::string name)
    {
        bindShared<InterfaceT, ImplementationT>(
            name, 
            RCF::SharedStubFactory<ImplementationT, InterfaceT>().makeServerStub(px));
    }

    template<typename InterfaceT, typename ImplementationT> 
    void RcfServer::bind(boost::shared_ptr<ImplementationT> px, std::string name)
    {
        bindShared<InterfaceT, ImplementationT>(
            name, 
            RCF::SharedStubFactory<ImplementationT, InterfaceT>().makeServerStub(px));
    }

    template<typename InterfaceT, typename ImplementationT> 
    void RcfServer::bindShared(
        std::string name, 
        boost::shared_ptr<I_ServerStub> serverStub)
    {
        RCF_ASSERT( !running_ );
                
        if (name == "")
        {
#if defined(__BORLANDC__) || defined(__MWERKS__)
            name = RCF_typename( (ImplementationT *) NULL ); // TODO: why the heck does this work???
#else
            name = RCF_typename( (InterfaceT *) NULL );
#endif
        }

        RCF_ASSERT( sharedStubTokens.find(name) == sharedStubTokens.end())(name);

        Token token = getTokenFactory().getNextToken();
        RCF_ASSERT( getStubTable().find(token) != getStubTable().end() );
        serverStub->setToken(token);
        getStubEntry(token).reset(serverStub, false );
        sharedStubTokens[name] = token;
        registerStubFactory(
            name, 
            "", 
            boost::shared_ptr<I_StubFactory>( new RCF::SharedStubFactory<ImplementationT, InterfaceT>() ));
    }

} // namespace RCF

#endif // ! _RCF_RCFSERVER_INL_
