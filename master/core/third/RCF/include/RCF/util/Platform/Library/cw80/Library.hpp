
#ifndef _UTIL_PLATFORM_LIBRARY_CW80_LIBRARY_HPP_
#define _UTIL_PLATFORM_LIBRARY_CW80_LIBRARY_HPP_

// Library specific stuff for metrowerks codewarrior 8.0

#if defined(SF_UTIL_DEBUG_NEW) && defined(new)
#undef new
#define REMEMBER_TO_TURN_DEBUGNEW_BACK_ON
#endif

#include <hash_map>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifdef REMEMBER_TO_TURN_DEBUGNEW_BACK_ON
#define new SF_UTIL_DEBUG_NEW
#undef REMEMBER_TO_TURN_DEBUGNEW_BACK_ON
#endif


namespace Platform {

    namespace Library {

        template<typename Key, typename Value, typename Hash>
        struct hash_map_t {
            typedef Metrowerks::hash_map<Key, Value, Hash> Val;
        };

    } // namespace Library

} // namespace Platform

#endif _UTIL_PLATFORM_LIBRARY_CW80_LIBRARY_HPP_
