
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_METHODINVOCATION_HPP_
#define _RCF_METHODINVOCATION_HPP_

#include <string>

#include <RCF/Token.hpp>

namespace RCF {

    class MethodInvocationRequest 
    {
    public:
        MethodInvocationRequest();
        MethodInvocationRequest(bool close);
        MethodInvocationRequest(const Token &token, const std::string &service, int fnId, bool oneway, bool close);
        Token getToken() const;
        int getFnId() const;
        bool getOneway() const;
        bool getClose() const;
        std::string getService() const { return service; }
        void setService(const std::string &service) { this->service = service; }

        template<typename Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar & token & fnId & oneway & close & service;
        }

    private: 
        //friend Connection &operator<<(Connection &connection, const MethodInvocationRequest &methodInvocationRequest);
        //friend Connection &operator>>(Connection &connection, MethodInvocationRequest &methodInvocationRequest);
        Token token;
        int fnId;
        bool oneway;
        bool close;
        std::string service;
    };

    class MethodInvocationResponse
    {
    public:
        MethodInvocationResponse();
        MethodInvocationResponse(bool ok);
        bool isException() const;
        bool isNotException() const;

        template<typename Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar & exception;
        }

    private: 
        bool exception;
    };

} // namespace RCF

#endif // ! _RCF_METHODINVOCATION_HPP_
