
#pragma once

#include "../inttype.h"
#include <string>

#if defined(_MSC_VER)
#include <Windows.h>
#else
#error("not support yet!!!")
#endif

namespace klib {
namespace io {


enum SEEK_ORIGIN
{
    SO_BEGINNING    = 0,
    SO_CURRENT      = 1,
    SO_END          = 2
};


/*
 * @brief 流的接口定义
 */
class istream
{
public:
    virtual ~istream() {}

    virtual int read(void *buffer, int count) = 0;
    virtual int write(const void *buffer, int count) = 0;
    virtual int64_t seek(int64_t offset, SEEK_ORIGIN seekOrigin) = 0;

    void read_buffer(void *buffer, int count);
    void write_buffer(const void *buffer, int count);

    int64_t get_pos() { return seek(0, SO_CURRENT); }
    void set_pos(int64_t pos) { seek(pos, SO_BEGINNING); }

    virtual int64_t get_size();


#define OPERATOR_IN(type) \
    istream &operator<< (const type value) { \
        this->write(&value, sizeof(value)); \
        return *this; \
    }

    OPERATOR_IN(INT8)
        OPERATOR_IN(INT16)
        OPERATOR_IN(INT32)
        OPERATOR_IN(INT64)
        OPERATOR_IN(UINT8)
        OPERATOR_IN(UINT16)
        OPERATOR_IN(UINT32)
        OPERATOR_IN(UINT64)
#undef OPERATOR_IN


#define OPERATOR_OUT(type) \
    istream &operator>>(type &value) { \
        this->read(&value, sizeof(value));\
        return *this; \
    }
        OPERATOR_OUT(INT8)
        OPERATOR_OUT(INT16)
        OPERATOR_OUT(INT32)
        OPERATOR_OUT(INT64)
        OPERATOR_OUT(UINT8)
        OPERATOR_OUT(UINT16)
        OPERATOR_OUT(UINT32)
        OPERATOR_OUT(UINT64)
#undef OPERATOR_OUT

};

/**
 * @brief 内存流 
 */
class memory_stream : public istream
{
public:
    enum { DEFAULT_MEMORY_DELTA = 1024 };    // 缺省内存增长步长 (字节数，必须是 2 的 N 次方)
    enum { MIN_MEMORY_DELTA = 256 };         // 最小内存增长步长

public:
    explicit memory_stream(int memoryDelta = DEFAULT_MEMORY_DELTA);
    memory_stream(const memory_stream& src);
    virtual ~memory_stream();

    memory_stream& operator = (const memory_stream& rhs);

    virtual int read(void *buffer, int count);
    virtual int write(const void *buffer, int count);
    virtual int64_t seek(int64_t offset, SEEK_ORIGIN seekOrigin);
    virtual void set_size(int64_t size);

    bool load(istream& stream);
    bool load(const std::string& fileName);
    bool save(istream& stream);
    bool save(const std::string& fileName);
    void clear();
    char* get_memory() { return memory_; }

private:
    void init();
    void assign(const memory_stream& src);
    void set_memory_delta(int newMemoryDelta);
    void set_pointer(char *memory, int size);
    void set_capacity(int newCapacity);
    char* realloc(int& newCapacity);

private:
    char *memory_;
    int capacity_;
    int size_;
    int position_;
    int memoryDelta_;
};


enum emFileMode
{
    FM_CREATE           = 0xFFFF,
    FM_OPEN_READ        = 0x0000,
    FM_OPEN_WRITE       = 0x0001,
    FM_OPEN_READ_WRITE  = 0x0002,

    FM_SHARE_EXCLUSIVE  = 0x0010,
    FM_SHARE_DENY_WRITE = 0x0020,
    FM_SHARE_DENY_NONE  = 0x0040
};

enum emFileAccess { DEFAULT_FILE_ACCESS_RIGHTS = 0 };

/**
 * @brief 文件流 
 */
class file_stream : public istream
{
public:
    file_stream();
    file_stream(const std::string& fileName, uint32_t openMode, uint32_t rights = DEFAULT_FILE_ACCESS_RIGHTS);
    virtual ~file_stream();

    bool open(const std::string& fileName, uint32_t openMode,
        uint32_t rights = DEFAULT_FILE_ACCESS_RIGHTS);
    void close();

    virtual int read(void *buffer, int count);
    virtual int write(const void *buffer, int count);
    virtual int64_t seek(int64_t offset, SEEK_ORIGIN seekOrigin);
    virtual void set_size(int64_t size);

    std::string get_filename() const { return fileName_; }
    HANDLE get_handle() const { return handle_; }
    bool is_open() const;

private:
    void init();
    HANDLE file_create(const std::string& fileName, uint32_t rights);
    HANDLE file_open(const std::string& fileName, uint32_t openMode);
    void file_close(HANDLE handle);
    int file_read(HANDLE handle, void *buffer, int count);
    int file_write(HANDLE handle, const void *buffer, int count);
    int64_t file_seek(HANDLE handle, int64_t offset, SEEK_ORIGIN seekOrigin);

private:
    std::string fileName_;
    HANDLE handle_;
};




}}

