
#ifndef _UTIL_PLATFORM_LIBRARY_BCC55_LIBRARY_HPP_
#define _UTIL_PLATFORM_LIBRARY_BCC55_LIBRARY_HPP_

// Library specific stuff for bcc 5.5

#include <stdlib.h>
#include <stlport/hash_map>

namespace Platform {

    namespace Library {

        template<typename Key, typename Type, typename Hash>
        struct hash_map_t {
            typedef std::hash_map<Key, Type, Hash> Val;
        };

        template<typename PtrT>    void ResetPtr(PtrT &ptr) { ptr.reset(); }
        template<typename PtrT, typename T>    void ResetPtr(PtrT &ptr, T *pt) { ptr.reset(pt); }

        inline size_t wcstombs(char *mbstr, const wchar_t *wcstr, size_t size) { return ::wcstombs(mbstr, wcstr, size); }
        inline size_t mbstowcs(wchar_t *wcstr, const char *mbstr, size_t size) { return ::mbstowcs(wcstr, mbstr, size); }

    } // namespace Library

} // namespace Platform

#define _timeb timeb
#define _ftime ftime

#endif _UTIL_PLATFORM_LIBRARY_BCC55_LIBRARY_HPP_
