//#include "stdafx.h"
#include "ini_file.h"

using namespace klib;
using namespace klib::core;


ini_file::ini_file(void)
{
}

ini_file::~ini_file(void)
{
}

void ini_file::set_file(const TCHAR* file)
{
	_tcsncpy(m_iniFile, file, _countof(m_iniFile));
}

tstring ini_file::get_string(TCHAR* AppName, TCHAR* KeyName, TCHAR*  default_val)
{
	TCHAR buf[MAX_PATH];
	::GetPrivateProfileString(AppName, KeyName, default_val, buf, sizeof(buf), m_iniFile);
	return tstring(buf);
}

double ini_file::get_double(TCHAR* AppName,TCHAR* KeyName,double default_val)
{
	TCHAR buf[MAX_PATH] = {0};
	TCHAR default_val_buff[200] = {0};
	_tprintf(default_val_buff, _T("%f"), default_val);
	::GetPrivateProfileString(AppName, KeyName, default_val_buff, buf, sizeof(buf), m_iniFile);
	return _ttof(buf);
}

int ini_file::get_int(TCHAR* AppName,TCHAR* KeyName,int default_val)
{
	return ::GetPrivateProfileInt(AppName, KeyName, default_val, m_iniFile);
}

unsigned long ini_file::get_dword(TCHAR* AppName,TCHAR* KeyName,unsigned long default_val)
{
	TCHAR buf[MAX_PATH];
	TCHAR default_val_buf[200] = {0};
	_stprintf(default_val_buf, _T("%u"), default_val);
	::GetPrivateProfileString(AppName, KeyName, default_val_buf, buf, sizeof(buf), m_iniFile);
	return _ttol(buf);
}

UINT64  ini_file::get_uint64(TCHAR* AppName, TCHAR* KeyName, UINT64 uDefault /*= 0*/)
{
    TCHAR buf[MAX_PATH];
    TCHAR default_val_buf[200] = {0};
    _stprintf(default_val_buf, _T("%I64"), uDefault);
    ::GetPrivateProfileString(AppName, KeyName, default_val_buf, buf, sizeof(buf), m_iniFile);
    return _ttoi64(buf);
}

BOOL  ini_file::get_bool(TCHAR* AppName, TCHAR* KeyName, BOOL default_val/* = false*/)
{
	tstring strVal = get_string(AppName, KeyName, default_val ? _T("true") : _T("false"));
	if (_ttol(strVal.c_str()) == 1) 
    {
		return true;
	}

	if (_tcsstr(strVal.c_str(), _T("true")) != NULL) 
    {
		return true;
	}
	
	return false;
}

BOOL ini_file::set_string(TCHAR* AppName,TCHAR* KeyName,TCHAR* val)
{
	return ::WritePrivateProfileString(AppName, KeyName, val, m_iniFile);
}

BOOL ini_file::set_int(TCHAR* AppName,TCHAR* KeyName,int val)
{
	TCHAR buff[MAX_PATH] = {0};
	_ltot(val, buff, 10);
	return ::WritePrivateProfileString(AppName, KeyName, buff, m_iniFile);
}

BOOL ini_file::set_double(TCHAR* AppName,TCHAR* KeyName,double val)
{
	TCHAR buff[MAX_PATH] = {0};
	_stprintf(buff, _T("%f"), val);
	return ::WritePrivateProfileString(AppName, KeyName, buff, m_iniFile);
}

BOOL ini_file::set_dword(TCHAR* AppName,TCHAR* KeyName, unsigned long val)
{
	TCHAR buff[MAX_PATH] = {0};
	_stprintf(buff, _T("%d"), val);
    
	return ::WritePrivateProfileString(AppName, KeyName, buff, m_iniFile);
}

BOOL ini_file::set_bool(TCHAR* AppName, TCHAR* KeyName, BOOL val)
{
	DWORD dwVal = val ? 1 : 0;
	return set_dword(AppName, KeyName, dwVal);
}

UINT64 ini_file::set_uint64(TCHAR* AppName, TCHAR* KeyName, UINT64 uVal)
{
    TCHAR buff[MAX_PATH] = {0};
    _i64tot(uVal, buff, 10);
    return ::WritePrivateProfileString(AppName, KeyName, buff, m_iniFile);
}