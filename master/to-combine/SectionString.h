#pragma once
#include <vector>
#include <algorithm>
#include <ctype.h>
using namespace std;

#define __upchar(_Char) ( (_Char) >= 'a' && (_Char) <= 'z' ? (_Char)-'a'+'A' : (_Char)) 

struct CSString
{
	CSString()
		:pData(NULL)
		,uLength(0)
	{

	}

	CSString(const char* pd, UINT32 len)
		:pData(pd)
		,uLength(len)
	{
	}
	
	const char* GetBegin() const
	{
		return pData;
	}

	const char* GetEnd() const
	{
		//assert(uLength < 1518);
		return pData + uLength;
	}

	int ToInt() const
	{
		char* pEnd = (char*)GetEnd();
		return strtol(pData, &pEnd, 10);
	}
	
	const char*	pData;
	size_t 		uLength;
};

typedef vector<CSString> CSStrings;

class CSectionString
{
	static bool CompareChar(char c1, char c2)
	{
		if (c1 == c2)
		{
			return true;
		}
		else
		{
			char ch1 = __upchar(c1);
			char ch2 = __upchar(c2);
			return ch1 == ch2;
		}
	}
// 	static BOOL CSStringToString(const CSString& cs,CStrW& string)
// 	{
// 		const UINT32 length = cs.uLength + 1;
// 		string.data()
// 		int nLength = MultiByteToWideChar(CP_ACP,0,cs.pData,cs.uLength, &pData, cs.uLength + 1);
// 		string = pData;
// 		return TRUE;
// 	}
public:
	CSectionString():m_uMaxCount(10000)
	{
		Clear();
	}
	
	virtual ~CSectionString(void){}

	void SetString(const char* pString, size_t uLength)
	{
		//assert(pString && uLength > 0);
		m_cstrSource.pData   = pString;
		m_cstrSource.uLength = uLength;
	}

	void SetString(const char* pBeginString, const char* pEndString)
	{
		assert(pBeginString && pEndString && (pEndString - pBeginString >= 0));
		m_cstrSource.pData   = pBeginString;
		m_cstrSource.uLength = (UINT32)(pEndString - pBeginString);
	}


	void SetString(const CSString& csString)
	{
		SetString(csString.pData, csString.uLength);
	}

	void SetDelimiter(char* pSeparator, UINT32 uLength)
	{
		m_cstrDelimiter.pData = pSeparator;
		m_cstrDelimiter.uLength = uLength;
	}

	void SetMaxCount(size_t nCount)
	{
		m_uMaxCount = nCount;
	}

	void Clear()
	{
		memset(&m_cstrSource, 0, sizeof(m_cstrSource));
		memset(&m_cstrDelimiter, 0, sizeof(m_cstrDelimiter));
		m_strings.clear();
	}

	size_t Process2()
	{
		const char* pPos = std::search( m_cstrSource.GetBegin(),
										m_cstrSource.GetEnd(),
										m_cstrDelimiter.GetBegin(),
										m_cstrDelimiter.GetEnd()
										);

		if (pPos != m_cstrSource.GetEnd())
		{
			CSString csString;
			//section 1
			csString.pData = m_cstrSource.GetBegin();
			csString.uLength = pPos - m_cstrSource.GetBegin();
			m_strings.push_back(csString);
			
			//section 2
			csString.pData = csString.GetEnd() + m_cstrDelimiter.uLength;
			if (csString.pData < m_cstrSource.GetEnd())
			{
				csString.uLength = m_cstrSource.GetEnd() - csString.pData;
				m_strings.push_back(csString);
			}
		}
		return m_strings.size();
	}

	size_t Execute()
	{
		m_strings.clear();
		if (m_cstrSource.uLength <= 0 || m_cstrSource.uLength <= m_cstrDelimiter.uLength)
		{
			return 0;
		}

		if (m_uMaxCount == 2)
		{
			return Process2();
		}

		const char* pBeginStr = m_cstrSource.pData;
		const char* const pEndStr  = m_cstrSource.GetEnd();
		const char* pPos = NULL;
		CSString csString;
		while(true)
		{
			
			pPos = std::search( pBeginStr,
								pEndStr,
								m_cstrDelimiter.GetBegin(),
								m_cstrDelimiter.GetEnd()
				);

			INT64 nLength = pPos - pBeginStr;
			//csString.uLength = (UINT32)(pPos - pBeginStr);
			
			if (nLength == 0)
			{
				break;
			}

			//²Ã¼ôÄ©Î²µÄ»Ø³µ»»ÐÐ·û
			if (*(pPos - 1) == '\n' || *(pPos - 1) == '\r' )
			{
				nLength--;
			}

			if (nLength <= 0)
			{
				break;
			}
			csString.uLength = (size_t)nLength;

			csString.pData = pBeginStr;
			m_strings.push_back(csString);
						
			if (m_strings.size() >= m_uMaxCount)
			{
				break;
			}

			pBeginStr = pPos + m_cstrDelimiter.uLength;

			if (pPos ==pEndStr || pBeginStr >= pEndStr)
			{
				break;
			}
		}
		return m_strings.size();
	}


	int  Find(const char* pSubstring, size_t uLength, bool fullMatch = true, size_t nOffset = 0)
	{
		return Find((UINT8*)pSubstring, uLength, fullMatch, nOffset);
	}

	int  Find(const UINT8* pSubstring, size_t uLength, bool fullMatch = true, size_t nOffset = 0)
	{
		if (uLength > m_cstrSource.uLength)
		{
			return -1;
		}

		const char* pPos = NULL;
		if (fullMatch)
		{
			pPos = std::search(m_cstrSource.pData + nOffset,
				m_cstrSource.GetEnd(),
				pSubstring,
				pSubstring + uLength);
		}
		else
		{
			pPos = std::search(m_cstrSource.pData + nOffset,
				m_cstrSource.GetEnd(),
				pSubstring,
				pSubstring + uLength,
				CompareChar);
		}

		if (pPos ==  m_cstrSource.GetEnd())
		{
			return -1;
		}
		else
		{
			return (int)(pPos - m_cstrSource.GetBegin());
		}
	}

	int  Compare(const char* pSubstring, size_t uLength = 0, bool fullMatch = false)
	{
		if (uLength == 0)
		{
			uLength = strlen(pSubstring);
		}

		if (m_cstrSource.uLength < uLength)
		{
			return -1;
		}
		
		if (fullMatch)
		{
			return strncmp(m_cstrSource.pData, pSubstring, uLength);
		}
		else
		{
			return _strnicmp(m_cstrSource.pData, pSubstring, uLength);
		}
	}

	int Compare(size_t nPos, const char* pSubstring, size_t uLength = 0, bool fullMatch = false)
	{
		if (uLength == 0)
		{
			uLength = strlen(pSubstring);
		}

		if (nPos >= m_cstrSource.uLength)
		{
			return -1;
		}
		
		if (m_cstrSource.uLength - nPos < uLength)
		{
			return -1;
		}

		if (fullMatch)
		{
			return strncmp(m_cstrSource.pData + nPos, pSubstring, uLength);
		}
		else
		{
			return _strnicmp(m_cstrSource.pData + nPos, pSubstring, uLength);
		}
	}

	int  CompareSection(size_t nIndex, const char* pSubstring, size_t uLength = 0, bool fullMatch = false)
	{
		if (m_strings.size() < nIndex)
		{
			return -1;
		}

		CSString& csString = m_strings[nIndex];
		if (csString.uLength < uLength)
		{
			return -1;
		}

		if (uLength == 0)
		{
			uLength = strlen(pSubstring);
		}

		if (fullMatch)
		{
			return strncmp(csString.pData, pSubstring, uLength);
		}
		else
		{
			return _strnicmp(csString.pData, pSubstring, uLength);
		}
	}

	CSStrings& GetSectionStrings() { return m_strings; }
	CSString   GetSectionFromIndex(size_t nIndex) { return m_strings.at(nIndex); }
	const CSString& GetSourceString() { return m_cstrSource; }
	CSString& operator[](size_t nIndex) { return m_strings.at(nIndex); }
private:
	size_t		m_uMaxCount;
	CSString    m_cstrSource;    //Ô­×Ö·û´®
	CSString	m_cstrDelimiter; //·Ö¸ô×Ö·û´®
	CSStrings   m_strings;
};

/*

CSectionString::CSectionString()
:m_uMaxCount(10000)
{
	Clear();
}

void CSectionString::SetString( const char* pString, UINT32 uLength )
{
	assert(pString && uLength > 0);
	m_cstrSource.pData   = pString;
	m_cstrSource.uLength = uLength;
}

void CSectionString::SetString( const CSString& csString )
{
	SetString(csString.pData, csString.uLength);
}
CSectionString::~CSectionString( void )
{

}
void CSectionString::SetDelimiter( char* pSeparator, UINT32 uLength )
{
	m_cstrDelimiter.pData = pSeparator;
	m_cstrDelimiter.uLength = uLength;
}

size_t CSectionString::Execute()
{
	m_strings.clear();
	const char* pStr = m_cstrSource.pData;
	//const char* pEnd = m_cstrSource.pData + m_cstrSource.uLength;
	const char* pPos = NULL;
	CSString csString;
	while(true)
	{

		pPos = std::search(pStr,
			m_cstrSource.GetEnd(),
			m_cstrDelimiter.GetBegin(),
			m_cstrDelimiter.GetEnd());

		csString.pData = pStr;
		csString.uLength = (UINT32)(pPos - pStr);
		m_strings.push_back(csString);

		if (m_strings.size() >= m_uMaxCount)
		{
			break;
		}

		if (pPos == m_cstrSource.GetEnd())
		{
			break;
		}
		pStr = pPos + m_cstrDelimiter.uLength;
	}
	return m_strings.size();
}

void CSectionString::SetMaxCount( size_t nCount )
{
	m_uMaxCount = nCount;
}



int CSectionString::Find(const char* pSubstring, size_t uLength, bool fullMatch )
{
	const char* pPos = NULL;
	if (fullMatch)
	{
		pPos = std::search(m_cstrSource.pData,
			m_cstrSource.pData + m_cstrSource.uLength,
			pSubstring,
			pSubstring + uLength);
	}
	else
	{
		pPos = std::search(m_cstrSource.GetBegin(),
			m_cstrSource.GetEnd(),
			pSubstring,
			pSubstring + uLength,
			CompareChar);
	}

	if (pPos ==  m_cstrSource.GetEnd())
	{
		return -1;
	}
	else
	{
		return (int)(pPos - m_cstrSource.GetBegin());
	}
}

int CSectionString::Compare( const char* pSubstring, size_t uLength, bool fullMatch )
{
	if (m_cstrSource.uLength < uLength)
	{
		return -1;
	}

	if (fullMatch)
	{
		return strncmp(m_cstrSource.pData, pSubstring, uLength);
	}
	else
	{
		return strnicmp(m_cstrSource.pData, pSubstring, uLength);
	}
}

int CSectionString::CompareSection( size_t nIndex, const char* pSubstring, size_t uLength , bool fullMatch )
{
	if (m_strings.size() < nIndex)
	{
		return -1;
	}

	CSString& csString = m_strings[nIndex];
	if (csString.uLength < uLength)
	{
		return -1;
	}

	if (uLength == 0)
	{
		uLength = strlen(pSubstring);
	}

	if (fullMatch)
	{
		return strncmp(csString.pData, pSubstring, uLength);
	}
	else
	{
		return strnicmp(csString.pData, pSubstring, uLength);
	}
}

void CSectionString::Clear()
{
	memset(&m_cstrSource, 0, sizeof(m_cstrSource));
	memset(&m_cstrDelimiter, 0, sizeof(m_cstrDelimiter));
	m_strings.clear();
}
*/