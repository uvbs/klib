
#ifndef _UTIL_PLATFORM_LIBRARY_MSVC60_LIBRARY_HPP_
#define _UTIL_PLATFORM_LIBRARY_MSVC60_LIBRARY_HPP_

// Library specific stuff for msvc 6.0

//#include <hash_map>
#include <map>

namespace Platform {

    namespace Library {

        template<typename Key, typename Type, typename HashFunction>
        struct hash_map_t {
            typedef std::map<Key, Type> Val;
        };

    } // namespace Library

} // namespace Platform

#endif _UTIL_PLATFORM_LIBRARY_MSVC60_LIBRARY_HPP_
