//========================================================================
// Minidump.cpp : This is a crash trapper - similar to a UNIX style core dump
//========================================================================


//#include "stdafx.h"
#include <time.h>
#include "mini_dumper.h"

#pragma warning(disable: 4996)
#pragma comment(lib, "version.lib")

using namespace klib::debug;


#define USER_DATA_BUFFER_SIZE (4096)

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
									CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
									CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
									CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
									);

mini_dumper *mini_dumper::gpDumper = NULL;

//
// mini_dumper::mini_dumper			
//
mini_dumper::mini_dumper(bool headless)
{
	// Detect if there is more than one mini_dumper.
	_ASSERT(!gpDumper);

	if (!gpDumper)
	{
		::SetUnhandledExceptionFilter( Handler );
		gpDumper = this;
		m_bHeadless = headless;						// doesn't throw up a dialog, just writes the file.
	}
}


//
// mini_dumper::Handler			
//
LONG mini_dumper::Handler(_EXCEPTION_POINTERS *pExceptionInfo)
{
	LONG retval = EXCEPTION_CONTINUE_SEARCH;

	if (!gpDumper)
	{
		return retval;
	}

	return gpDumper->WriteMiniDump(pExceptionInfo);
}

//
// mini_dumper::WriteMiniDump			
//
LONG mini_dumper::WriteMiniDump(_EXCEPTION_POINTERS *pExceptionInfo )
{    
    GetLocalTime(&m_stSysTime);

	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	m_pExceptionInfo = pExceptionInfo;

	// You have to find the right dbghelp.dll. 
	// Look next to the EXE first since the one in System32 might be old (Win2k)
	
	HMODULE hDll = NULL;
    TCHAR szDbgHelpPath[_MAX_PATH] = {0};

	if (GetModuleFileName(NULL, m_szAppPath, _MAX_PATH))
	{
		TCHAR *pSlash = _tcsrchr( m_szAppPath, '\\' );
		
		if (pSlash)
		{
            memset(m_szAppBaseName, 0, sizeof(m_szAppBaseName));
			_tcsncpy(m_szAppBaseName, pSlash + 1, _tcslen(pSlash + 1));
			*(pSlash + 1) = 0;
		}

		_tcsncpy(szDbgHelpPath, m_szAppPath, _tcslen(m_szAppPath));
        _tcsncat(szDbgHelpPath, _T("DBGHELP.DLL"), sizeof(_T("DBGHELP.DLL")) - 1);
		hDll = ::LoadLibrary(szDbgHelpPath);
	}

	if (hDll==NULL)
	{
		// If we haven't found it yet - try one more time.
		hDll = ::LoadLibrary(_T("DBGHELP.DLL"));
	}

	LPCTSTR szResult = NULL;

	if (hDll)
	{
		MINIDUMPWRITEDUMP pMiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");

		if (pMiniDumpWriteDump)
		{
			TCHAR szScratch[USER_DATA_BUFFER_SIZE];

			VSetDumpFileName();

			// ask the user if they want to save a dump file
			//_tcssprintf(szScratch, _T("There was an unexpected error:\n\n%s\nWould you like to save a diagnostic file?\n\nFilename: %s"), VGetUserMessage(), m_szDumpPath);
			wsprintf(szScratch, _T("There was an unexpected error:\n\n%s\nWould you like to save a diagnostic file?\n\nFilename: %s"), VGetUserMessage(), m_szDumpPath);
			if (m_bHeadless || (::MessageBox( NULL, szScratch, NULL, MB_YESNO )==IDYES))
			{
				// create the file
				HANDLE hFile = ::CreateFile( m_szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
											FILE_ATTRIBUTE_NORMAL, NULL );

				if (hFile!=INVALID_HANDLE_VALUE)
				{
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

					ExInfo.ThreadId = ::GetCurrentThreadId();
					ExInfo.ExceptionPointers = pExceptionInfo;
					ExInfo.ClientPointers = NULL;

					// write the dump
					BOOL bOK = pMiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, VGetUserStreamArray(), NULL );
					if (bOK)
					{
						szResult = NULL;
						retval = EXCEPTION_EXECUTE_HANDLER;
					}
					else
					{
						//_tcssprintf( szScratch, _T("Failed to save dump file to '%s' (error %d)"), m_szDumpPath, GetLastError() );
						wsprintf( szScratch, _T("Failed to save dump file to '%s' (error %d)"), m_szDumpPath, GetLastError() );
						szResult = szScratch;
					}
					::CloseHandle(hFile);
				}
				else
				{
					//_tcssprintf( szScratch, _T("Failed to create dump file '%s' (error %d)"), m_szDumpPath, GetLastError() );
					wsprintf( szScratch, _T("Failed to create dump file '%s' (error %d)"), m_szDumpPath, GetLastError() );
					szResult = szScratch;
				}
			}
		}
		else
		{
			szResult = _T("DBGHELP.DLL too old");
		}
	}
	else
	{
		szResult = _T("DBGHELP.DLL not found");
	}

	if (szResult && !m_bHeadless)
		::MessageBox( NULL, szResult, NULL, MB_OK );

	TerminateProcess(GetCurrentProcess(), 0);
	
	// MLM Note: ExitThread will work, and it allows the mini_dumper to kill a crashed thread
	// without affecting the rest of the application. The question of the day:
	//   Is That A Good Idea??? Answer: ABSOLUTELY NOT!!!!!!!
	//
	//ExitThread(0);

	return retval;
}

//
// mini_dumper::VSetDumpFileName				
//
void mini_dumper::VSetDumpFileName(void)
{
	//_tcssprintf(m_szDumpPath, _T("%s%s.%ld.dmp"), m_szAppPath, m_szAppBaseName, m_lTime);
	wsprintf(m_szDumpPath,
            _T("%s%s.%d-%d-%d %d-%d-%d.dmp"), 
            m_szAppPath, 
            m_szAppBaseName, 
            m_stSysTime.wYear,
            m_stSysTime.wMonth,
            m_stSysTime.wDay,
            m_stSysTime.wHour,
            m_stSysTime.wMinute,
            m_stSysTime.wSecond);
}

