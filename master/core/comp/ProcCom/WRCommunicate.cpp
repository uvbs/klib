//#include "stdafx.h"
#include "WRCommunicate.h"
#include <tchar.h>

CWRCommunicate::CWRCommunicate(tstring name,int buffersize):
m_semaphore(name), m_name(name), m_bEnd(false)
{
	m_memroy.create(name + _T("_sharebuffer"), buffersize);
}

CWRCommunicate::~CWRCommunicate(void)
{
}

void CWRCommunicate::write(string& str)
{
	if(m_bEnd)
    {
		return ;
	}
    else
	{
		m_semaphore.write_wait();
		m_memroy.write_string(str);
		m_semaphore.read_set();
	}
}

void CWRCommunicate::read(string& str)
{
	m_semaphore.read_wait();
	if(m_bEnd)
    {
		str = "REXIT";
	}
    else
    {
		str = m_memroy.read_string();
	}
	m_semaphore.write_set();
}

bool CWRCommunicate::isEnd()
{
	return m_bEnd;
}

tstring CWRCommunicate::getname()
{
	return m_name;
}

void CWRCommunicate::close()
{
	m_bEnd = true;
	m_semaphore.read_set();
}
