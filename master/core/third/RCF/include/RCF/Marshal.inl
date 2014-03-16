
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_MARSHAL_INL_
#define _RCF_MARSHAL_INL_

#include <RCF/Marshal.hpp>
#include <RCF/Exception.hpp>
#include <RCF/Tools.hpp>

namespace RCF {
    namespace IDL {

        template<typename U>
        InReturnValue<U>::InReturnValue(Connection &connection, bool oneway) : t_() 
        {
            connection.send();
            
            if (!oneway) 
            {
                int err = connection.receive();

                RCF_ASSERT(err >= -2);
                if (err == 0)
                {
                    connection.close();
                    RCF_THROW( RCF::RecvCloseException, "connection closed by server" );
                }
                else if (err == -1)
                {
                    connection.close();
                    RCF_THROW( RCF::RecvException, "failed to receive response from server")(err)(Platform::OS::GetErrorString(err));
                }
                else if (err == -2)
                {
                    connection.close();
                    unsigned int timeoutMs = connection.getRecvTimeoutMs();
                    RCF_THROW( RCF::RecvTimeoutException, "server response timed out")(timeoutMs);
                }

                RCF::MethodInvocationResponse response;
                connection >> response;
                if (response.isException()) 
                {
                    std::string remoteExceptionType;
                    std::string remoteExceptionMsg;
                    connection >> remoteExceptionType >> remoteExceptionMsg;
                    throw RCF::RemoteException(remoteExceptionType, remoteExceptionMsg);
                }
                connection >> t_;
            }
        }

        template<typename T>
        T &InReturnValue<T>::get()
        {
            return t_;
        }

        
        inline InReturnValue<Void>::InReturnValue(Connection &connection, bool oneway) 
        {
            connection.send();
            if (!oneway) 
            {
                int err = connection.receive();

                RCF_ASSERT(err >= -2);
                if (err == 0)
                {
                    RCF_THROW( RCF::RecvCloseException, "connection closed by server" );
                }
                else if (err == -1)
                {
                    RCF_THROW( RCF::RecvException, "failed to receive response from server")(err)(Platform::OS::GetErrorString(err));
                }
                else if (err == -2)
                {
                    RCF_THROW( RCF::RecvTimeoutException, "server response timed out");
                }
                
                RCF::MethodInvocationResponse response;
                connection >> response;
                if (response.isException()) 
                {
                    std::string remoteExceptionType;
                    std::string remoteExceptionMsg;
                    connection >> remoteExceptionType >> remoteExceptionMsg;
                    throw RCF::RemoteException(remoteExceptionType, remoteExceptionMsg);
                }
            }
        }

        
    } // namespace IDL

} // namespace RCF

#endif // ! _RCF_MARSHAL_INL_
