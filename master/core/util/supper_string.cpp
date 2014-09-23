#include "supper_string.h"

using namespace klib::util;


#pragma warning (disable : 4996)





mstring::mstring()
{
    initstring();
}

mstring::mstring(const char *buffer)
{
    assert(buffer);
    initstring();
    *this = buffer;
}

mstring::mstring(const char *buffer, size_t len)
{
    assert(buffer);
    initstring();
    this->append(buffer, len);
}

mstring::mstring(const std::string &buffer)
{
    initstring();
    *this = buffer.c_str();
}

mstring::mstring(const std::wstring &buffer)
{
    initstring();
    *this = ustring(buffer).trans_gbk();
}

mstring::mstring(const wchar_t *buffer)
{
    assert(buffer);
    initstring();
    *this = ustring(buffer).trans_gbk();
}

mstring::mstring(const wchar_t *buffer, size_t len)
{
    assert(buffer);
    initstring();
    *this = ustring(buffer, len).trans_gbk();
}

mstring::~mstring()
{
    if (m_buffer)
    {
        free(m_buffer);
        m_buffer = 0;
        m_size = 0;
    }
}

void mstring::initstring()
{
    m_buffer = 0;
    m_size = 0;
}

mstring::operator const char *()
{
    return this->c_str();
}

mstring &mstring::operator = (const char *buffer)
{
    assert(buffer);
    this->erase(this->begin(), this->end());
    this->append(buffer);
    return *this;
}

mstring &mstring::operator = (const wchar_t *buffer)
{
    assert(buffer);
    this->erase(this->begin(), this->end());
    this->append(ustring(buffer).trans_gbk());
    return *this;
}

mstring &mstring::operator = (char c)
{
    this->erase(this->begin(), this->end());
    this->append(1, c);
    return *this;
}

mstring &mstring::operator = (wchar_t w)
{
    this->erase(this->begin(), this->end());
    wchar_t ws[2] = {w, 0x0000};
    *this = ws;
    return *this;
}

mstring &mstring::operator += (char c)
{
    this->append(1, c);
    return *this;
}

mstring &mstring::operator += (wchar_t w)
{
    wchar_t ws[2] = {w, 0x0000};
    *this += ws;
    return *this;
}

mstring &mstring::operator += (const char *buffer)
{
    assert(buffer);
    this->append(buffer);
    return *this;
}

mstring &mstring::operator += (const wchar_t *buffer)
{
    assert(buffer);
    this->append(ustring(buffer).trans_gbk());
    return *this;
}

mstring &mstring::setbuffer(size_t len)
{
    assert(m_buffer);
    this->erase(this->begin(), this->end());
    if (len > 0)
    {
        size_t size = len > m_size ? m_size : len;
        this->append(m_buffer, size);
    }
    else
    {
        this->append(m_buffer);
    }
    return *this;
}

mstring &mstring::format(const char *format, ...)
{
    assert(format);
    va_list arg;
    char *buffer = 0;
    size_t size = 0;

    buffer = (char *)malloc(sizeof(char) * 512);
    if (!buffer)
    {
        return *this;
    }
    size = 512;
    va_start(arg, format);
    int length = vsnprintf(buffer, size, format, arg);
    while(length < 0)
    {
        size *= 2;
        buffer = (char *)realloc(buffer, size);
        if (!buffer)
        {
            break;
        }
        length = vsnprintf(buffer, size, format, arg);
    }
    va_end(arg);
    if (buffer)
    {
        this->assign(buffer, length);
        free(buffer);
    }
    return *this;
}

mstring &mstring::makelower()
{
    transform(this->begin(), this->end(), this->begin(), ::tolower); 
    return *this;
}

mstring &mstring::makeupper()
{
    transform(this->begin(), this->end(), this->begin(), ::toupper); 
    return *this;
}

char *mstring::alloc(size_t size)
{
    if (m_size >= size)
    {
        return m_buffer;
    }

    m_size = (size + 127) & ~127;
    if (m_buffer)
    {
        if (!(m_buffer = (char *)realloc(m_buffer, m_size)))
        {
            m_size = 0;
        }
    }
    else
    {
        if (!(m_buffer = (char *)malloc(m_size)))
        {
            m_size = 0;
        }
    }
    return m_buffer;
}

void mstring::release()
{
    if (m_buffer)
    {
        free(m_buffer);
        m_size = 0;
    }
}

mstring &mstring::delsub(const char *sub)
{
    assert(sub);
    int itm = 0;
    int flag = 0;
    size_t length = strlen(sub);
    while((flag = this->find(sub, itm)) != std::string::npos)
    {
        this->erase(flag, length);
        itm = flag;
    }
    return *this;
}

mstring &mstring::delchar(char c)
{
    int itm = 0;
    int flag = 0;
    while((flag = this->find(c, itm)) != std::string::npos)
    {
        this->erase(flag, 1);
        itm = flag;
    }
    return *this;
}

mstring &mstring::trimleft()
{
    mstring::iterator itm = this->begin();
    while(itm != this->end())
    {
        if (*itm == '\r' || *itm == '\n' || *itm == ' ')
        {
            itm = this->erase(itm);
        }
        else
        {
            break;
        }
    }
    return *this;
}

mstring &mstring::trimright()
{
    size_t it = this->size() - 1;
    while(it >= 0)
    {
        if (this->at(it) == '\r' || this->at(it) == '\n' || this->at(it) == ' ')
        {
            this->erase(it);
            it--;
        }
        else
        {
            break;
        }
    }
    return *this;
}

mstring &mstring::base64encode()
{
    static const char s_encodetable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int databyte = this->size();
    mstring encode;
    unsigned char tmp[4] = {0};
    int LineLength = 0;
    int it = 0;
    int im = 0;
    for(im = 0 ; im < (int)(databyte / 3) ; im++)
    {
        tmp[1] = this->at(it++);
        tmp[2] = this->at(it++);
        tmp[3] = this->at(it++);
        encode += s_encodetable[tmp[1] >> 2];
        encode += s_encodetable[((tmp[1] << 4) | (tmp[2] >> 4)) & 0x3F];
        encode += s_encodetable[((tmp[2] << 2) | (tmp[3] >> 6)) & 0x3F];
        encode += s_encodetable[tmp[3] & 0x3F];
        if(LineLength += 4, LineLength == 76) 
        {
            encode+="\r\n";LineLength = 0;
        }
    }
    int mod = databyte % 3;
    if(mod == 1)
    {
        tmp[1] = *this[it++];
        encode+= s_encodetable[(tmp[1] & 0xFC) >> 2];
        encode+= s_encodetable[((tmp[1] & 0x03) << 4)];
        encode+= "==";
    }
    else if(mod == 2)
    {
        tmp[1] = this->at(it++);
        tmp[2] = this->at(it++);
        encode+= s_encodetable[(tmp[1] & 0xFC) >> 2];
        encode+= s_encodetable[((tmp[1] & 0x03) << 4) | ((tmp[2] & 0xF0) >> 4)];
        encode+= s_encodetable[((tmp[2] & 0x0F) << 2)];
        encode+= "=";
    }
    *this = encode;
    return *this;
}

mstring &mstring::base64decode()
{
    static const char s_decodetable[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        62, // '+'
        0, 0, 0,
        63, // '/'
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
        0, 0, 0, 0, 0, 0,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
        39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
    };

    mstring decode;
    int value;
    int i = 0;
    int im = 0;
    int out = 0;
    while ((size_t)i < this->size())
    {
        if (this->at(im) != '\r' && this->at(im) != '\n')
        {
            value = s_decodetable[this->at(im++)] << 18;
            value += s_decodetable[this->at(im++)] << 12;
            decode += char((value & 0x00FF0000) >> 16);
            out++;
            if (this->at(im) != '=')
            {
                value += s_decodetable[this->at(im++)] << 6;
                decode += char((value & 0x0000FF00) >> 8);
                out++;
                if (this->at(im) != '=')
                {
                    value += s_decodetable[this->at(im++)];
                    decode += (char)(value & 0x000000FF);
                    out++;
                }
            }
            i += 4;
        }
        else
        {
            im++;
            i++;
        }
    }
    *this = decode;
    return *this;
}

mstring &mstring::trans_utf8()
{
    ustring us;
    us = trans_unicode();
    *this = us.trans_utf8();
    return *this;
}

ustring mstring::trans_unicode()
{
    ustring uf;
    int count = MultiByteToWideChar(CP_ACP, 0, this->c_str(), -1, NULL, 0);
    if (count > 0)
    {
        wchar_t *buffer = (wchar_t *)malloc(sizeof(wchar_t) * count);
        if (buffer != 0)
        {
            MultiByteToWideChar(CP_ACP, 0, this->c_str(), -1, buffer, count);
            uf = buffer;
            free(buffer);
        }
    }
    return uf;
}

ustring::ustring()
{
    initstring();
}

ustring::ustring(const wchar_t *buffer)
{
    assert(buffer);
    initstring();
    *this = buffer;
}

ustring::ustring(const wchar_t* buffer, size_t len)
{
    assert(buffer);
    initstring();
    this->append(buffer, len);
}

ustring::ustring(const std::wstring &buffer)
{
    initstring();
    *this = buffer.c_str();
}

ustring::ustring(const char *buffer)
{
    assert(buffer);
    initstring();
    *this = buffer;
}

ustring::ustring(const char *buffer, size_t len)
{
    assert(buffer);
    initstring();
    *this = mstring(buffer, len).trans_unicode();
}

ustring::ustring(const std::string &buffer)
{
    initstring();
    *this = mstring(buffer).trans_unicode();
}

void ustring::initstring()
{
    m_buffer = 0;
    m_size = 0;
}

ustring::~ustring()
{
    if (m_buffer)
    {
        free(m_buffer);
        m_buffer = 0;
        m_size = 0;
    }
}

ustring &ustring::format(const wchar_t *format, ...)
{
    assert(format);
    wchar_t *buffer = 0;
    int length;
    int size;
    va_list arg;

    buffer = (wchar_t *)malloc(sizeof(wchar_t) * 512);
    if (!buffer)
    {
        size = 0;
        return *this;
    }
    size = 512;
    va_start(arg, format);
    length = vswprintf(buffer, size, format, arg);
    while(length < 0)
    {
        size *= 2;
        buffer = (wchar_t *)realloc(buffer, m_size);
        if (!buffer)
        {
            break;
        }
        length = vswprintf(buffer, size, format, arg);
    }
    va_end(arg);
    if (buffer)
    {
        this->assign(buffer, length);
        free(buffer);
    }
    return *this;
}

ustring::operator const wchar_t *()
{
    return this->c_str();
}

ustring &ustring::operator = (const wchar_t *buffer)
{
    assert(buffer);
    this->erase(this->begin(), this->end());
    this->append(buffer);
    return *this;
}

ustring &ustring::operator = (wchar_t w)
{
    this->erase(this->begin(), this->end());
    this->append(1, w);
    return *this;
}

ustring &ustring::operator += (const wchar_t *buffer)
{
    assert(buffer);
    this->append(buffer);
    return *this;
}

ustring &ustring::operator += (wchar_t w)
{
    this->append(1, w);
    return *this;
}

ustring &ustring::operator += (const char *buffer)
{
    assert(buffer);
    this->append(mstring(buffer).trans_unicode());
    return *this;
}

ustring &ustring::operator = (const char *buffer)
{
    assert(buffer);
    *this = mstring(buffer).trans_unicode();
    return *this;
}

ustring &ustring::operator += (char c)
{
    char cs[2] = {c, 0x00};
    *this += mstring(cs).trans_unicode();
    return *this;
}

ustring &ustring::operator = (char c)
{
    char cs[2] = {c, 0x00};
    *this = mstring(cs).trans_unicode();
    return *this;
}

ustring &ustring::makelower()
{
    transform(this->begin(), this->end(), this->begin(), ::tolower); 
    return *this;
}

ustring &ustring::makeupper()
{
    transform(this->begin(), this->end(), this->begin(), ::toupper); 
    return *this;
}

wchar_t *ustring::alloc(size_t size)
{
    if (m_size >= size)
    {
        return m_buffer;
    }

    m_size = (size + 127) & ~127;
    if (m_buffer)
    {
        if (!(m_buffer = (wchar_t *)realloc(m_buffer, sizeof(wchar_t) * size)))
        {
            m_size = 0;
        }
    }
    else
    {
        if (!(m_buffer = (wchar_t *)malloc(sizeof(wchar_t) * size)))
        {
            m_size = 0;
        }
    }
    return m_buffer;
}

ustring &ustring::setbuffer(size_t len)
{
    assert(m_buffer);
    this->erase(this->begin(), this->end());
    if (len > 0)
    {
        size_t size = len > m_size ? m_size : len;
        this->append(m_buffer, size);
    }
    else
    {
        this->append(m_buffer);
    }
    return *this;
}

void ustring::release()
{
    if (m_buffer)
    {
        free(m_buffer);
        m_buffer = 0;
        m_size = 0;
    }
}

ustring &ustring::delsub(const wchar_t *sub)
{
    assert(sub);
    int itm = 0;
    int flag = 0;
    size_t length = wcslen(sub);
    while((flag = this->find(sub, itm)) != ustring::npos)
    {
        this->erase(flag, length);
        itm = flag;
    }
    return *this;
}

ustring &ustring::delchar(char c)
{
    int itm = 0;
    int flag = 0;
    while((flag = this->find(c, itm)) != std::wstring::npos)
    {
        this->erase(flag, 1);
        itm = flag;
    }
    return *this;
}

ustring &ustring::trimleft()
{
    ustring::iterator itm = this->begin();
    while(itm != this->end())
    {
        if (*itm == '\r' || *itm == '\n' || *itm == ' ')
        {
            itm = this->erase(itm);
        }
        else
        {
            break;
        }
    }
    return *this;
}

ustring &ustring::trimright()
{
    size_t it = this->size() - 1;
    while(it >= 0)
    {
        if (this->at(it) == L'\r' || this->at(it) == L'\n' || this->at(it) == L' ')
        {
            this->erase(it);
            it--;
        }
        else
        {
            break;
        }
    }
    return *this;
}

ustring &ustring::base64encode()
{
    mstring gbk = trans_gbk();
    gbk.base64encode();
    *this = gbk.trans_unicode();
    return *this;
}

ustring &ustring::base64decode()
{
    mstring gbk = trans_gbk();
    gbk.base64decode();
    *this = gbk.trans_unicode();
    return *this;
}

mstring ustring::trans_utf8()
{
    mstring ret;
    int count = WideCharToMultiByte(CP_UTF8, 0, this->c_str(), -1, NULL, 0, NULL, NULL);
    if (count > 0)
    {
        char *buffer = (char *)malloc(sizeof(char) *count);
        if (buffer != 0)
        {
            WideCharToMultiByte(CP_UTF8, 0, this->c_str(), -1, buffer, count, NULL, NULL);
            ret = buffer;
            free(buffer);
        }
    }
    return ret;
}

mstring ustring::trans_gbk()
{
    mstring ret;
    int count = WideCharToMultiByte(CP_ACP, 0, this->c_str(), -1, NULL, 0, NULL, NULL);
    if (count > 0)
    {
        char *buffer = (char *)malloc(sizeof(char) * count);
        if (buffer != 0)
        {
            WideCharToMultiByte(CP_ACP, 0, this->c_str(), -1, buffer, count, NULL, NULL);
            ret = buffer;
            free(buffer);
        }
    }
    return ret;
}

