
#ifndef _UTIL_PLATFORM_LIBRARY_MSVC71_LIBRARY_HPP_
#define _UTIL_PLATFORM_LIBRARY_MSVC71_LIBRARY_HPP_

// Library specific stuff for msvc 7.1

#include <hash_map>

namespace Platform {

    namespace Library {

        template<typename K, typename HashFunction>
        class hash_compare : public stdext::hash_compare<K> {
        public:
            size_t operator()(const K &a) const { return hash(a); }
            bool operator()(const K &lhs, const K &rhs) const { return lhs < rhs; }
        private:
            HashFunction hash;
        };

        template<typename Key, typename Type, typename HashFunction>
        struct hash_map_t {
            typedef stdext::hash_map<Key, Type, hash_compare<Key, HashFunction> > Val;
        };

    } // namespace Library

} // namespace Platform


#endif _UTIL_PLATFORM_LIBRARY_MSVC71_LIBRARY_HPP_
