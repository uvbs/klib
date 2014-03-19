
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_EXCEPTION_HPP_
#define _SF_EXCEPTION_HPP_

#include <stdexcept>
#include <string>

namespace SF {

    // Generic exception class - all exceptions thrown by SF must derive from it
    class Exception : public std::runtime_error
    {
    public:
        Exception(std::string msg = "Unspecified") : std::runtime_error(""), msg_(msg) {}
        ~Exception() throw() {}
        const char *what() const throw() { return msg_.c_str(); }
    private:
        std::string msg_;
    };

} // namespace SF

#endif // ! _SF_EXCEPTION_HPP_
