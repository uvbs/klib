#ifndef _local_archive_H
#define _local_archive_H

#include "net_archive.h"

namespace klib {
namespace net {



template<int iBuffLen = 1024>
class local_archive : public net_archive
{
public:
	local_archive() : net_archive(m_chWriteBuff, sizeof(m_chWriteBuff))
	{
		m_chWriteBuff[0] = 0;
	}

private:
	char m_chWriteBuff[iBuffLen];
};



}}




#endif