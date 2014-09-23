#ifndef _klib_supper_string_h_
#define _klib_supper_string_h_


#include <algorithm>
#include <Windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>


#include <string>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

//@todo  finish supper string implement 

namespace klib{
namespace util{

class supper_string
{
public:

protected:

};


/*
*	filename:  mstring.h
*	author:    lougd
*	created:   2014-7-15 11:38
*	version:   1.0.0.1
*	desc:	  封装了C++标准库中的string类和wstring类，添加了一些常用的方法	
*/

/*
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include "mstring.h"

using namespace std;

void FunTest1(const char *p)
{
cout << p << endl;
}

void FunTest2(char *p, size_t len)
{
strcpy_s(p, len, "ni hao");
cout << p << endl;
}

int main()
{
mstring mbuffer;
ustring ubuffer;

mbuffer = L"Hello";
mbuffer += "World";
cout << "mstring : "<<mbuffer << endl;

ubuffer = mbuffer.c_str();
cout << "ustring : " << L"xxxx" << endl;

ubuffer.delchar(L'o');
cout << "ustring : " << ubuffer << endl;

mbuffer += "   \r\n";
mbuffer.insert(0, "  ");
cout << "mstring : "<< mbuffer << "size = " << mbuffer.size() << endl;

mbuffer.trimleft();
mbuffer.trimright();

cout << "mbuffer : "<<  mbuffer << "size = " << mbuffer.size() << endl;

mbuffer.delsub("ld");
cout << "mbuffer : "<< mbuffer << endl;

cout << "mbuffer : " << "base64 encode = " << mbuffer.base64encode() << endl;
cout << "mbuffer : " << "base64 decode = " << mbuffer.base64decode() << endl;

FunTest1(mbuffer);
FunTest2(mbuffer.alloc(16), 16);
mbuffer.setbuffer();
cout << "mbuffer : " << mbuffer << endl;

cout << "mbuffer : " << mbuffer.format("xxx = %d", 1234) << endl;
MessageBoxA(0, 0, 0, 0);
return 0;
}
*/


class ustring;
class mstring : public std::string
{
public:
    mstring();
    mstring(const char* buffer);
    mstring(const char* buffer, size_t len);
    mstring(const std::string &buffer);
    mstring(const std::wstring &buffer);
    mstring(const wchar_t *buffer);
    mstring(const wchar_t *buffer, size_t len);
    virtual ~mstring();

public:
    mstring &format(const char *pFormat, ...);
    operator const char *();
    mstring &operator = (const char *buffer);
    mstring &operator = (const wchar_t *buffer);
    mstring &operator = (char c);
    mstring &operator = (wchar_t w);
    mstring &operator += (const char *buffer);
    mstring &operator += (const wchar_t *buffer);
    mstring &operator += (char c);
    mstring &operator += (wchar_t w);
    mstring &makelower();
    mstring &makeupper();
    char *alloc(size_t size);
    mstring &setbuffer(size_t len = 0);
    void release();
    mstring &delsub(const char *sub);
    mstring &delchar(char c);
    mstring &trimleft();
    mstring &trimright();
    mstring &base64encode();
    mstring &base64decode();
    mstring &trans_utf8();
    ustring trans_unicode();

protected:
    void initstring();

protected:
    char *m_buffer;
    size_t m_size;
};

class ustring : public std::wstring
{
public:
    ustring();

    ustring(const char* buffer);
    ustring(const char* buffer, size_t len);
    ustring(const std::string &buffer);
    ustring(const std::wstring &buffer);
    ustring(const wchar_t *buffer);
    ustring(const wchar_t *buffer, size_t len);
    virtual ~ustring();

public:
    ustring &format(const wchar_t *format, ...);
    operator const wchar_t *();
    ustring &operator = (const char *buffer);
    ustring &operator = (const wchar_t *buffer);
    ustring &operator = (char c);
    ustring &operator = (wchar_t w);
    ustring &operator += (const char *buffer);
    ustring &operator += (const wchar_t *buffer);
    ustring &operator += (char c);
    ustring &operator += (wchar_t w);

    ustring &makelower();
    ustring &makeupper();
    wchar_t *alloc(size_t size);
    ustring &setbuffer(size_t size = 0);
    void release();
    ustring &delsub(const wchar_t *sub);
    ustring &delchar(char c);
    ustring &trimleft();
    ustring &trimright();
    ustring &base64encode();
    ustring &base64decode();
    mstring trans_utf8();
    mstring trans_gbk();

protected:
    void initstring();

protected:
    wchar_t *m_buffer;
    size_t m_size;
};



}}



#endif // _klib_supper_string_h_