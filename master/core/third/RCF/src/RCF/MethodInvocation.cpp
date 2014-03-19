
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#include <RCF/MethodInvocation.hpp>

namespace RCF {

    //*************************************
    // MethodInvocationRequest

    MethodInvocationRequest::MethodInvocationRequest() 
        : token(), fnId(), oneway(), close(), service()
    {}

    MethodInvocationRequest::MethodInvocationRequest(bool close) 
        : token(), fnId(), oneway(), close(close), service()
    {}

    MethodInvocationRequest::MethodInvocationRequest(const Token &token, const std::string &service, int fnId, bool oneway, bool close)
        : token(token), fnId(fnId), oneway(oneway), close(close), service(service)
    {}

    Token MethodInvocationRequest::getToken() const 
    { 
        return token; 
    }
    
    int MethodInvocationRequest::getFnId() const 
    { 
        return fnId; 
    }

    bool MethodInvocationRequest::getOneway() const 
    { 
        return oneway; 
    }
    
    bool MethodInvocationRequest::getClose() const 
    { 
        return close; 
    }

    
    //*******************************************'
    // MethodInvocationResponse

    MethodInvocationResponse::MethodInvocationResponse() 
        : exception() 
    {} 

    MethodInvocationResponse::MethodInvocationResponse(bool exception) 
        : exception(exception)
    {}

    bool MethodInvocationResponse::isException() const 
    { 
        return exception; 
    }
    
    bool MethodInvocationResponse::isNotException() const 
    { 
        return !exception;
    }

} // namespace RCF
