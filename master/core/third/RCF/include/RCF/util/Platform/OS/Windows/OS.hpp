// OS-specific details - Windows

#ifndef _UTIL_PLATFORM_OS_WINDOWS_OS_HPP_
#define _UTIL_PLATFORM_OS_WINDOWS_OS_HPP_

// With Codewarrior we get multiple redefinition problems when including <winsock2.h> and <windows.h>.
// Easiest way aound seems to be a project-wide definition of WIN32_LEAN_AND_MEAN.

#ifdef WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#else
//#define WIN32_LEAN_AND_MEAN
//#include <winsock2.h>
#include <windows.h>
//#undef WIN32_LEAN_AND_MEAN
#endif

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>

#ifndef __WINDOWS__
#define __WINDOWS__
#endif

#if !defined(NDEBUG) && !defined(_DEBUG)
#define _DEBUG
#endif

namespace Platform {

    namespace OS {

        inline void OutputDebugString(const char *sz) 
        { 
            ::OutputDebugString(sz); 

#ifdef UTIL_OUTPUTDEBUGSTRING_TO_STDERR
            fprintf(stderr, "%s", sz);
#endif

#ifdef UTIL_OUTPUTDEBUGSTRING_TO_STDOUT
            fprintf(stdout, "%s", sz);
#endif

#ifdef UTIL_OUTPUTDEBUGSTRING_TO_FILE
            static FILE *file = fopen( "OutputDebugString.txt", "w" );
            fprintf(file, "%s", sz);
#endif

        }

        inline std::string GetErrorString(int err)
        {
            std::string errorString = "Error string lookup failed";
            LPVOID lpMsgBuf;
            if (FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                err/*GetLastError()*/,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (char *) &lpMsgBuf,
                0,
                NULL ))
            {
                errorString = (const char *) lpMsgBuf;
                LocalFree( lpMsgBuf );
            }

            return errorString;
        }

        inline std::string GetErrorString() { return GetErrorString( ::GetLastError() ); }

        inline int GetCurrentThreadId() { return ::GetCurrentThreadId(); }

        inline void Sleep(unsigned int seconds) { return ::Sleep(1000*seconds); }
        
        namespace Socket {

            typedef ::SOCKET SOCKET;

            inline int recv(int fd, char *buf, int len, int flags)               
            { 
                return ::recv(fd, buf, len, flags); 
            }

            inline int send(int fd, const char *buf, int len, int flags)
            { 
                return ::send(fd, buf, len, flags); 
            }

            inline int accept(int fd, sockaddr *addr, int *addrlen)
            { 
                return (int) ::accept(fd, addr, addrlen); 
            }

            inline int connect(int fd, const sockaddr *name, int namelen)        
            { 
                return ::connect(fd, name, namelen); 
            }

            inline int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timeval *timeout) 
            { 
                return ::select(nfds, readfds, writefds, exceptfds, timeout ); 
            }

            inline int closesocket(int fd)                                       
            { 
                return ::closesocket(fd); 
            }

            inline void setblocking(int fd, bool bBlocking)                      
            { 
                u_long arg = bBlocking ? 0 : 1; 
                ioctlsocket(fd, FIONBIO, &arg); 
            }

            inline int GetLastError()                                               
            { 
                return ::WSAGetLastError(); 
            }

            static const int ERR_EWOULDBLOCK = WSAEWOULDBLOCK;
            static const int ERR_EINPROGRESS = WSAEINPROGRESS;
            static const int ERR_ECONNRESET = WSAECONNRESET;
            static const int ERR_ECONNABORTED = WSAECONNABORTED;
            static const int ERR_ECONNREFUSED = WSAECONNREFUSED;

            
        } //namespace Socket

    } // namespace OS;

} // namespace Platform

#endif //! _UTIL_PLATFORM_OS_WINDOWS_OS_HPP_
