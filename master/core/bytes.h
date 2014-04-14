#ifndef BYTES_H
#define BYTES_H

#include <stdint.h>

#define __BIG_ENDIAN 4321
#define __LITTLE_ENDIAN 1234
#define __BYTE_ORDER 1234

/* Swap bytes in 16 bit value.  */
#define __bswap_constant_16(x) \
  ((((x) >> 8) & 0xffu) | (((x) & 0xffu) << 8))
/* Swap bytes in 32 bit value.  */
#define __bswap_constant_32(x) \
  ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >>  8) |	      \
  (((x) & 0x0000ff00u) <<  8) | (((x) & 0x000000ffu) << 24))
/* Swap bytes in 64 bit value.  */
# define __bswap_constant_64(x) \
  ((((x) & 0xff00000000000000ull) >> 56)				      \
  | (((x) & 0x00ff000000000000ull) >> 40)				      \
  | (((x) & 0x0000ff0000000000ull) >> 24)				      \
  | (((x) & 0x000000ff00000000ull) >> 8)				      \
  | (((x) & 0x00000000ff000000ull) << 8)				      \
  | (((x) & 0x0000000000ff0000ull) << 24)				      \
  | (((x) & 0x000000000000ff00ull) << 40)				      \
  | (((x) & 0x00000000000000ffull) << 56))

#define bswap_8(x) ((uint8_t)(x))
inline uint16_t bswap_16(uint16_t x) { return __bswap_constant_16(x); }
inline uint32_t bswap_32(uint32_t x) { return __bswap_constant_32(x); }
inline uint64_t bswap_64(uint64_t x) { return __bswap_constant_64(x); }

#if __BYTE_ORDER == __BIG_ENDIAN
#define ntoh8(x) ((uint8_t)(x))
#define ntoh16(x) ((uint16_t)(x))
#define ntoh32(x) ((uint32_t)(x))
#define ntoh64(x) ((uint64_t)(x))
#define hton8(x) ((uint8_t)(x))
#define hton16(x) ((uint16_t)(x))
#define hton32(x) ((uint32_t)(x))
#define hton64(x) ((uint64_t)(x))
#else
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define ntoh8(x) bswap_8(x)
#define ntoh16(x) bswap_16(x)
#define ntoh32(x) bswap_32(x)
#define ntoh64(x) bswap_64(x)
#define hton8(x) bswap_8(x)
#define hton16(x) bswap_16(x)
#define hton32(x) bswap_32(x)
#define hton64(x) bswap_64(x)
#endif
#endif

#endif  // BYTES_H
