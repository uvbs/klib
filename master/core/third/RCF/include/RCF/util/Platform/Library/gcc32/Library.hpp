
#ifndef _UTIL_PLATFORM_LIBRARY_GCC32_LIBRARY_HPP_
#define _UTIL_PLATFORM_LIBRARY_GCC32_LIBRARY_HPP_

// Library specific stuff for gcc 3.2

#include <ext/hash_map>

namespace Platform {

    namespace Library {

        template<typename Key, typename Value, typename Hash>
        struct hash_map_t {
            typedef __gnu_cxx::hash_map<Key, Value, Hash> Val;
        };

    } // namespace Library

} // namespace Platform

#ifndef __MINGW32__
#define _timeb timeb
#define _ftime ftime
#endif

#endif // ! _UTIL_PLATFORM_LIBRARY_GCC32_LIBRARY_H_
