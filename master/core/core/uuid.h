#ifndef _klib_uuid_h_
#define _klib_uuid_h_


#include "../inttype.h"

#ifdef WIN32
#else
#include <uuid/uuid.h>
#endif

namespace klib{
namespace core{

inline string create_uuid()
{
    char buf[64] = {0};
#ifdef WIN32
    GUID guid;
    if (S_OK == CoCreateGuid(&guid))
    {
        _snprintf(buf, sizeof(buf),
            "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1],
            guid.Data4[2], guid.Data4[3],
            guid.Data4[4], guid.Data4[5],
            guid.Data4[6], guid.Data4[7]);
    }
#else
    uuid_t uu;  
    uuid_generate(uu);
    if (!uuid_is_null(uu)) 
    {
        snprintf(buf, sizeof(buf),
            "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
            uu[0], uu[1], uu[2], uu[3], uu[4], uu[5], uu[6], uu[7], 
            uu[8], uu[9], uu[10], uu[11], uu[12], uu[13], uu[14], uu[15]);
    }
#endif

    return buf;
}

}}

#endif // _klib_uuid_h_