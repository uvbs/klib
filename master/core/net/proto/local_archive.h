#ifndef _local_archive_H
#define _local_archive_H

#include "net_archive.h"
#include "tlv_archive.h"

namespace klib {
namespace net {



template<int iBuffLen = 1024>
class local_archive : public net_archive
{
public:
	local_archive() : net_archive(archive_buff_, sizeof(archive_buff_))
	{
		archive_buff_[0] = 0;
	}

private:
	char archive_buff_[iBuffLen];
};


template<int iBuffLen = 1024>
class local_tlv_archive : public tlv_archive
{
public:
    local_tlv_archive()
    {
        tlv_archive::set_buff(archive_buff_, sizeof(archive_buff_));
    }

protected:
    char archive_buff_[iBuffLen];
};


}}




#endif