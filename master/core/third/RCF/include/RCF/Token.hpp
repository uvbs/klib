
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_TOKEN_HPP_
#define _RCF_TOKEN_HPP_

#include <iosfwd>

namespace RCF {

    class Connection;

    class Token
    {
    public:
        Token();
        Token(int id);
        int getId() const;
        friend bool operator<(const Token &lhs, const Token &rhs);
        friend bool operator==(const Token &lhs, const Token &rhs);
        friend bool operator!=(const Token &lhs, const Token &rhs);
        
        template<typename Archive> void serialize(Archive &ar, const unsigned int)
        {
            ar & id;
        }
        friend std::ostream &operator<<(std::ostream &os, const Token &token);

    private:
        int id;
    };

} // namespace RCF

#endif // ! _RCF_TOKEN_HPP_
