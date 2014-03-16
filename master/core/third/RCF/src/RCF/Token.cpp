
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#include <RCF/Token.hpp>

#include <iostream>

namespace RCF {

    //*****************************************
    // Token

    Token::Token() 
        : id()
    {}

    bool operator<(const Token &lhs, const Token &rhs) 
    { 
        return (lhs.getId() < rhs.getId());
    }

    bool operator==(const Token &lhs, const Token &rhs) 
    {
        return lhs.getId() == rhs.getId();
    }

    bool operator!=(const Token &lhs, const Token &rhs) 
    {
        return ! (lhs == rhs);
    }

    int Token::getId() const 
    { 
        return id; 
    }
    
    std::ostream &operator<<(std::ostream &os, const Token &token)
    {
        os << "( id = " << token.getId() << " )";
        return os;
    }

    Token::Token(int id) 
        : id(id)
    {}
    
} // namespace RCF
