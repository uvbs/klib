
#ifndef _UTIL_PLATFORM_OS_WINDOWS_SOCKETINIT_HPP_
#define _UTIL_PLATFORM_OS_WINDOWS_SOCKETINIT_HPP_

#include <RCF/util/InitDeinit.hpp>

namespace Platform {

    namespace OS {

        namespace Socket {

            inline void initWinsock()
            {
                WORD wVersion = MAKEWORD( 1, 0 );
                WSADATA wsaData;
                int err = WSAStartup( wVersion, &wsaData );
                assert( err == 0 ); // TODO: do something more useful
            }

            inline void deinitWinsock()
            {
                WSACleanup();
            }

            UTIL_ON_INIT_DEINIT( initWinsock(), deinitWinsock() )

        } // namespace Socket

    } // namespace OS

} // namespace Platform

#endif // ! _UTIL_PLATFORM_OS_WINDOWS_SOCKETINIT_HPP_
