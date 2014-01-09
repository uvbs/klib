#ifndef _klib_encode_crc32_h_
#define _klib_encode_crc32_h_



#include "../istddef.h"
#include "../inttype.h"


namespace klib{
namespace encode {

/* crc32–£—È¿‡ */
class crc32
{
public:
    crc32();

    void init_table();
    uint32_t get_crc(const unsigned char* data, size_t size, uint32_t crc = 0) ;

    USHORT check_sum(USHORT* buff, int size);

protected:
    uint32_t crc32_table_[256];
};

}}


#endif  // _klib_encode_crc32_h_