
/**
    Created 2007.6.7 by PeakGao
    Email: peakgao163@163.com
*/

#ifndef _klib_IOBUFFER_H__
#define _klib_IOBUFFER_H__

#include <assert.h>
#include "inttype.h"

/** 示例.
 @code
    struct SimpleStruct
    {
        Uint x,y;
    };

    void test()
    {
        SimpleStruct ss;
        ss.x = 11111;
        ss.y = 22222;
        Uint8 v8 = 8;
        Uint16 v16 = 16;
        Uint32 v32 = 32;
        Uint v = 123456789;
        Float vf = 3.1415926f;
        Double vd = 3.1415926535897932;
        long vl = 0xffffffff;
        unsigned long vul = 0xffffffff;
        const char* name = "user name";
        Uint8 mydata[12] = {0,1,2,3,4,5,6,7,8,9,10,11};

        char data[256];
        obuffer out(data, sizeof(data)); // 或者使用： FixOutBuffer<256> out;
        out << ss << name << v8 << v16 << v32 << v << vf << vd << vl << vul; // 导入固定长度类型的数据到缓冲区
        out.push(mydata, sizeof(mydata)); // 导入一个数组（缓冲区）到该缓冲区
        if (!out)
        {
            Error("error\n");
        }

        ibuffer in(out.buffer(), out.size());
        ss.x = ss.y = 0;
        const char* nameptr = 0;
        memset(mydata, 0, sizeof(mydata));
        in >> ss >> nameptr >> v8 >> v16;
        in.skip<Uint32>();    // 跳过一个Uint32，比如我们不需要这个参数
        in >> v >> vf >> vd >> vl >> vul;
        in.pop(mydata, sizeof(mydata));
        if (!in)
        {
            Error("error");
        }
    }
 @endcode
 */

/// 缓冲区基类
class basic_buffer
{
public:
    enum bufstate{ _good = 0x0, _eof = 0x1, _fail = 0x2, _bad = 0x4/*严重错误*/};

protected:
    void*        _buf;        /// 缓冲区首地址
    size_t        _bufSize;    /// 缓冲区大小
    size_t        _pos;        /// 当前操作位置
    bufstate    _state;        /// 缓冲区当前操作的状态

    // 构造（析构）
protected:
    basic_buffer() : _buf(0), _bufSize(0), _pos(0), _state(_good) { }

public:
    basic_buffer(void* buffer, size_t maxsize) : _buf(buffer), _bufSize(maxsize), _pos(0), _state(_good)
    {
        assert(buffer != 0 && maxsize > 0);
    }

    ~basic_buffer() { }

    // 状态相关（用于检测操作的结果）
public:
    bool operator !() const
    {
        return (!good());
    }

    operator bool() const
    {
        return (good());
    }

    bufstate state() const
    {
        return _state;
    }

    void setstate(bufstate state_)
    {
        if (state_ != _good)
            _state = (bufstate)((int)_state | (int)state_);
    }

    void setstate(int state_)
    {
        setstate((bufstate)state_);
    }

    bool good() const
    {
        return ((int)state() == (int)_good || (int)state() == (int)(_good | _eof));
    }

    bool eof() const
    {
        return ((int)state() & (int)_eof);
    }

    bool fail() const
    {
        return (((int)state() & ((int)_fail | (int)_bad)) != 0);
    }

    bool bad() const
    {
        return (((int)state() & (int)_bad) != 0);
    }


    // 属性及操作
public:
    /// 缓冲区清除操作
    void clear()
    {
        memset(_buf, 0, _bufSize);
        _pos = 0;
        _state = _good;
    }

    /// 将当前位置向后移动指定的大小
    void skipn(size_t bytes)
    {
        if (_pos + bytes <= _bufSize)
        {
            _pos += bytes;

            if (_pos == _bufSize)
                setstate(basic_buffer::_eof);
        }
        else
            setstate(basic_buffer::_eof|basic_buffer::_fail);
    }

    /// 获取缓冲区地址
    void* buffer() const
    {
        return _buf;
    }

    /// 获取缓冲区当前位置的地址
    void* current() const
    {
        return (char*)_buf + _pos;
    }

    /// 获取缓冲区数据操作的当前位置偏移
    size_t pos() const
    {
        return _pos;
    }

    /// 获取缓冲区的容量（即缓冲区的大小）
    size_t capacity() const
    {
        return _bufSize;
    }
};




/// 输出缓冲（指各种数据导入到缓冲区中）
class obuffer : public basic_buffer
{
    // 构造（析构）
protected:
    obuffer() : basic_buffer() { }

public:
    obuffer(void* buffer, size_t maxsize) : basic_buffer(buffer, maxsize)    { }

    ~obuffer() { }


    // 方法
public:
    /// 获取缓冲区中数据的实际占用尺寸
    size_t size() const
    {
        return pos();
    }

    /// 向缓冲区写入各种具有固定长度的数据类型，包括简单类型和复合类型（结构）
    template <class T> obuffer& operator << (T value)
    {
        if (_pos + sizeof(T) <= _bufSize)
        {
            *(T*)((char*)_buf + _pos) = value;
            _pos += sizeof(T);

            if (_pos == _bufSize)
                setstate(basic_buffer::_eof);
        }
        else
        {
            setstate(basic_buffer::_eof|basic_buffer::_fail);
        }
        return (*this);
    }

//使用GCC编译时,该段代码可能要移植到buffer.cpp文件中分离编译
#ifdef _MSC_VER

    /// 对常字符串的特化处理
    template<> obuffer& operator << (const char* value)
    {
        return push((void*)value, strlen(value) + sizeof(char));
    }
    
    /// 对字符串的特化处理
    template<> obuffer& operator << (char* value)
    {
        return push((void*)value, strlen(value) + sizeof(char));
    }

    /// 对常宽字符串的特化处理
    template<> obuffer& operator << (const wchar_t* value)
    {
        return push((void*)value, (wcslen(value) + 1) * sizeof(wchar_t));
    }

    /// 对宽字符串的特化处理
    template<> obuffer& operator << (wchar_t* value)
    {
        return push((void*)value, (wcslen(value) + 1) * sizeof(wchar_t));
    }

#endif

    /// 跳过某种数据类型（不进行赋值，仅仅改变当前位置）
    template <class T> obuffer& skip()
    {
        if (_pos + sizeof(T) <= _bufSize)
        {
            _pos += sizeof(T);

            if (_pos == _bufSize)
                setstate(basic_buffer::_eof);
        }
        else
        {
            setstate(basic_buffer::_eof|basic_buffer::_fail);
        }
        return (*this);
    }

    /// 在当前位置导入一段缓冲
    obuffer& push(void* buffer, size_t bytes)
    {
        if (buffer == 0 || bytes == 0)
        {
            setstate(basic_buffer::_bad|basic_buffer::_fail);
            return (*this);
        }

        if (_pos + bytes <= _bufSize)
        {
            memcpy((char*)_buf + _pos, buffer, bytes);
            _pos += bytes;

            if (_pos == _bufSize)
                setstate(basic_buffer::_eof);
        }
        else
        {
            setstate(basic_buffer::_eof|basic_buffer::_fail);
        }
        return (*this);
    }
};


/// 固定大小的输出缓冲
template <size_t _MaxBytes = 256>
class FixOutBuffer : public obuffer
{
protected:
    enum {_Buf_Size = _MaxBytes ? ((_MaxBytes + 7) & ~7) : 8}; // 8字节对齐
    char _data[_Buf_Size];

public:
    FixOutBuffer()
    {
        _buf = _data;
        _bufSize = _Buf_Size;
        _pos = 0;
        _state = basic_buffer::_good;
    }

    ~FixOutBuffer() { }
};




/// 输入缓冲（指从缓冲区中导出各种数据）
class ibuffer : public basic_buffer
{
    // 构造（析构）
protected:
    ibuffer() : basic_buffer() { }

public:
    ibuffer(void* buffer, size_t actualSize) : basic_buffer(buffer, actualSize)    { }

    ~ibuffer() { }


    // 方法
public:
    /// 获取缓冲区中数据的实际占用尺寸
    size_t size() const
    {
        return _bufSize;
    }

    /// 从缓冲区导出各种具有固定长度的数据类型，包括简单类型和复合类型（结构）
    template <class T> ibuffer& operator >> (T& value)
    {
        if (_pos + sizeof(T) <= _bufSize)
        {
            value = *(T*)((char*)_buf + _pos);
            _pos += sizeof(T);

            if (_pos == _bufSize)
                setstate(basic_buffer::_eof);
        }
        else
        {
            setstate(basic_buffer::_eof|basic_buffer::_fail);
        }
        return (*this);
    }

//使用GCC编译时,该段代码可能要移植到buffer.cpp文件中分离编译
#ifdef _MSC_VER
    /// 对常字符串的特化处理
    template<> ibuffer& operator >> (const char*& value)
    {
        const char* str = (const char*)_buf + _pos;
        while ((size_t)(str - (const char*)_buf) < _bufSize && *str++);
        size_t bytes = (size_t)(str - (char*)_buf) - _pos;
        if (bytes > 0 && _pos + bytes <= _bufSize)
        {
            if (*((char*)_buf + _pos + bytes - 1) != 0) // 不是0结尾的字符串
            {
                setstate(basic_buffer::_eof|basic_buffer::_bad);
                return (*this);
            }
            value = (char*)_buf + _pos;
            _pos += bytes;

            if (_pos == _bufSize)
                setstate(basic_buffer::_eof);
        }
        else
        {
            setstate(basic_buffer::_eof|basic_buffer::_fail);
            value = 0;
        }

        return (*this);
    }

    /// 对字符串的特化处理
    template<> ibuffer& operator >> (char*& value)
    {
        const char* str = (const char*)_buf + _pos;
        while ((size_t)(str - (const char*)_buf) < _bufSize && *str++);
        size_t bytes = (size_t)(str - (char*)_buf) - _pos;
        if (bytes > 0 && _pos + bytes <= _bufSize)
        {
            if (*((char*)_buf + _pos + bytes - 1) != 0) // 不是0结尾
            {
                setstate(basic_buffer::_eof|basic_buffer::_bad);
                return (*this);
            }
            value = (char*)_buf + _pos;
            _pos += bytes;

            if (_pos == _bufSize)
                setstate(basic_buffer::_eof);
        }
        else
        {
            setstate(basic_buffer::_eof|basic_buffer::_fail);
            value = 0;
        }

        return (*this);
    }

    /// 对常宽字符串的特化处理
    template<> ibuffer& operator >> (const wchar_t*& value)
    {
        const wchar_t* str = (const wchar_t*)((uint8_t*)_buf + _pos);
        while ((size_t)((uint8_t*)str - (uint8_t*)_buf) < _bufSize && *str++);
        size_t bytes = (size_t)((uint8_t*)str - (uint8_t*)_buf) - _pos;
        if (bytes > 0 && _pos + bytes <= _bufSize)
        {
            if (*(const wchar_t*)((uint8_t*)_buf + _pos + bytes - sizeof(wchar_t)) != 0) // 不是0结尾
            {
                setstate(basic_buffer::_eof|basic_buffer::_bad);
                return (*this);
            }
            value = (const wchar_t*)((uint8_t*)_buf + _pos);
            _pos += bytes;

            if (_pos == _bufSize)
                setstate(basic_buffer::_eof);
        }
        else
        {
            setstate(basic_buffer::_eof|basic_buffer::_fail);
            value = 0;
        }

        return (*this);
    }

    /// 对宽字符串的特化处理
    template<> ibuffer& operator >> (wchar_t*& value)
    {
        const wchar_t* str = (const wchar_t*)((uint8_t*)_buf + _pos);
        while ((size_t)((uint8_t*)str - (uint8_t*)_buf) < _bufSize && *str++);
        size_t bytes = (size_t)((uint8_t*)str - (uint8_t*)_buf) - _pos;
        if (bytes > 0 && _pos + bytes <= _bufSize)
        {
            if (*(const wchar_t*)((uint8_t*)_buf + _pos + bytes - sizeof(wchar_t)) != 0) // 不是0结尾
            {
                setstate(basic_buffer::_eof|basic_buffer::_bad);
                return (*this);
            }
            value = (wchar_t*)((uint8_t*)_buf + _pos);
            _pos += bytes;

            if (_pos == _bufSize)
                setstate(basic_buffer::_eof);
        }
        else
        {
            setstate(basic_buffer::_eof|basic_buffer::_fail);
            value = 0;
        }

        return (*this);
    }
#endif

    /// 跳过某种数据类型（不进行赋值，仅仅改变当前位置）
    template <class T> ibuffer& skip()
    {
        if (_pos + sizeof(T) <= _bufSize)
        {
            _pos += sizeof(T);

            if (_pos == _bufSize)
                setstate(basic_buffer::_eof);
        }
        else
            setstate(basic_buffer::_eof|basic_buffer::_fail);
        return (*this);
    }

    /// 在当前位置导出一段缓冲
    ibuffer& pop(void* buffer, size_t bytes)
    {
        if (buffer == 0 || bytes == 0)
        {
            setstate(basic_buffer::_bad|basic_buffer::_fail);
            return (*this);
        }

        if (_pos + bytes <= _bufSize)
        {
            memcpy(buffer, (char*)_buf + _pos, bytes);
            _pos += bytes;

            if (_pos == _bufSize)
                setstate(basic_buffer::_eof);
        }
        else
        {
            setstate(basic_buffer::_eof|basic_buffer::_fail);
        }
        return (*this);
    }
};


#ifndef FIXOUTBUFFER_TYPEDEF
#    define FIXOUTBUFFER_TYPEDEF
    typedef FixOutBuffer<32>    obuffer32;
    typedef FixOutBuffer<64>    obuffer64;
    typedef FixOutBuffer<256>    obuffer256;
    typedef FixOutBuffer<512>    obuffer512;
    typedef FixOutBuffer<1024>    obuffer1024;
    typedef FixOutBuffer<2048>    obuffer2048;
    typedef FixOutBuffer<4096>    obuffer4096;
#endif


#endif // _klib_IOBUFFER_H__

