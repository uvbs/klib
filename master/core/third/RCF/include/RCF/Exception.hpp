
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_EXCEPTIONS_HPP_
#define _RCF_EXCEPTIONS_HPP_

#include <stdexcept>
#include <string>

namespace RCF {

    // Generic exception class - all exceptions thrown by RCF derive from it
    class Exception : public std::runtime_error {
    public:
        Exception(const std::string &msg = "RCF::Exception: no msg") : std::runtime_error(msg) {}
        ~Exception() throw() {}
    };

    // RemoteException - thrown locally when a remote exception occurs
    class RemoteException : public Exception
    {
    public:
        RemoteException(const std::string &remoteExceptionType, const std::string &remoteExceptionMsg) :
          Exception( "Remote exception: Type: " + remoteExceptionType + ", Msg: " + remoteExceptionMsg)
        {}
        ~RemoteException() throw() {}
    };


#define RCF_DEFINE_EXCEPTION(E, PE)                                     \
    class E : public PE {                                               \
    public:                                                             \
        E(const std::string &msg = #E ": no msg") : PE(msg) {}          \
        ~E() throw() {}                                                 \
    };


    RCF_DEFINE_EXCEPTION(SerializationException,        Exception);
    RCF_DEFINE_EXCEPTION(ConnectionException,           Exception);
    RCF_DEFINE_EXCEPTION(SendException,                 Exception);
    RCF_DEFINE_EXCEPTION(RecvException,                 Exception);
    RCF_DEFINE_EXCEPTION(RecvCloseException,            RecvException);
    RCF_DEFINE_EXCEPTION(RecvTimeoutException,          RecvException);
    RCF_DEFINE_EXCEPTION(ServerException,               Exception);
    RCF_DEFINE_EXCEPTION(MultiplexerException,          ServerException);
    RCF_DEFINE_EXCEPTION(AssertionFailureException,     Exception);
    RCF_DEFINE_EXCEPTION(SessionInfoException,            Exception);

#undef RCF_DEFINE_EXCEPTION

} // namespace RCF

#endif // ! _RCF_EXCEPTIONS_HPP_
