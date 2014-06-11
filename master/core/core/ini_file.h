#ifndef _klib_ini_file_h
#define _klib_ini_file_h

#include <tchar.h>
#include <string>
#include <windows.h>

#include "../istddef.h"

namespace klib{
namespace core {


class ini_file
{
public:
	ini_file(void);
	~ini_file(void);

public:
	void set_file(const TCHAR* file);
	const TCHAR* get_file() { return m_iniFile; }

	tstring get_string(const TCHAR* AppName,const TCHAR* KeyName,const TCHAR* default_val_val = _T(""));
	int get_int(const TCHAR* AppName,const TCHAR* KeyName,int default_val = 0);
	double get_double(const TCHAR* AppName,const TCHAR* KeyName,double default_val = 0);
	unsigned long get_dword(const TCHAR* AppName,const TCHAR* KeyName,unsigned long default_val = 0);
    UINT64  get_uint64(const TCHAR* AppName, const TCHAR* KeyName, UINT64 uDefault = 0);
	BOOL  get_bool(const TCHAR* AppName, const TCHAR* KeyName, BOOL default_val = false);

	BOOL set_string(const TCHAR* AppName,const TCHAR* KeyName,const TCHAR* val);
	BOOL set_int(const TCHAR* AppName,const TCHAR* KeyName,int val);
	BOOL set_double(const TCHAR* AppName,const TCHAR* KeyName,double val);
	BOOL set_dword(const TCHAR* AppName,const TCHAR* KeyName,unsigned long val);
	BOOL set_bool(const TCHAR* AppName, const TCHAR* KeyName, BOOL val);
    UINT64  set_uint64(const TCHAR* AppName, const TCHAR* KeyName, UINT64 uVal);

protected:
	TCHAR m_iniFile[MAX_PATH*2];
};


}}



#endif
