#pragma once

#include "../../kthread/rw_semaphore.h"
#include "../../core/share_memory.h"
#include "../../istddef.h"

using namespace klib;
using namespace klib::mem;
using namespace klib::kthread;

/**
 * @brief  ¶ÁÐ´Í¨ÐÅÄ£¿é
 */
class CWRCommunicate
{
public:
	CWRCommunicate(tstring name,int buffersize);
	~CWRCommunicate(void);
    
	void write(string& str);
	void read(string& str);

	bool isEnd();
	tstring getname();
	void close();

private:
	tstring             m_name;
	wr_semaphore        m_semaphore;
	share_memory        m_memroy;
	bool                m_bEnd;
};
