#pragma once

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
	TCHAR* get_file() { return m_iniFile; }

	tstring get_string(TCHAR* AppName,TCHAR* KeyName,TCHAR* default_val_val = _T(""));
	int get_int(TCHAR* AppName,TCHAR* KeyName,int default_val = 0);
	double get_double(TCHAR* AppName,TCHAR* KeyName,double default_val = 0);
	unsigned long get_dword(TCHAR* AppName,TCHAR* KeyName,unsigned long default_val = 0);
    UINT64  get_uint64(TCHAR* AppName, TCHAR* KeyName, UINT64 uDefault = 0);
	BOOL  get_bool(TCHAR* AppName, TCHAR* KeyName, BOOL default_val = false);

	BOOL set_string(TCHAR* AppName,TCHAR* KeyName,TCHAR* val);
	BOOL set_int(TCHAR* AppName,TCHAR* KeyName,int val);
	BOOL set_double(TCHAR* AppName,TCHAR* KeyName,double val);
	BOOL set_dword(TCHAR* AppName,TCHAR* KeyName,unsigned long val);
	BOOL set_bool(TCHAR* AppName, TCHAR* KeyName, BOOL val);
    UINT64  set_uint64(TCHAR* AppName, TCHAR* KeyName, UINT64 uVal);

protected:
	TCHAR m_iniFile[MAX_PATH*2];
};


}}

