
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_CONNECTION_INL_
#define _RCF_CONNECTION_INL_

#include <RCF/Connection.hpp>

#include <RCF/Tools.hpp>

namespace RCF {

    template<typename T>
    void Connection::write(const T &t)
    {
        try
        {
            switch (protocol) 
            {
            case 1: oprotocol1 << t; break;
            case 2: oprotocol2 << t; break;
            case 3: oprotocol3 << t; break;
            case 4: oprotocol4 << t; break;
            case 5: oprotocol5 << t; break;
            case 6: oprotocol6 << t; break;
            case 7: oprotocol7 << t; break;
            case 8: oprotocol8 << t; break;
            case 9: oprotocol9 << t; break;
            case 10: oprotocol10 << t; break;
            default: RCF_ASSERT(0);
            }
        }
        catch(const std::exception &e)
        {
            RCF_THROW(
                RCF::SerializationException, 
                "serialization error: Type: " + std::string(typeid(e).name()) + 
                ", Msg: " + std::string(e.what()));
        }
    }

    template<typename T>
    void Connection::read(T &t)
    {
        try
        {
            switch (protocol) 
            {
            case 1: iprotocol1 >> t; break;
            case 2: iprotocol2 >> t; break;
            case 3: iprotocol3 >> t; break;
            case 4: iprotocol4 >> t; break;
            case 5: iprotocol5 >> t; break;
            case 6: iprotocol6 >> t; break;
            case 7: iprotocol7 >> t; break;
            case 8: iprotocol8 >> t; break;
            case 9: iprotocol9 >> t; break;
            case 10: iprotocol10 >> t; break;
            default: RCF_ASSERT(0);
            }
        }
        catch(const std::exception &e)
        {
            RCF_THROW(
                RCF::SerializationException, 
                "serialization error: Type: " + std::string(typeid(e).name()) + 
                ", Msg: " + std::string(e.what()));
        }
    }

    template<typename T> 
    Connection &operator<<(Connection &connection, const T &t) 
    {
        connection.write(t);
        return connection;
    }

    template<typename T> 
    Connection &operator>>(Connection &connection,       T &t) 
    {
        connection.read(t);
        return connection;
    }

} // namespace RCF

#endif // ! _RCF_CONNECTION_INL_
