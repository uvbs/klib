

#include "stream.h"

namespace klib{
namespace io {

union Int64Rec
{
    int64_t value;
    struct {
        INT32 lo;
        INT32 hi;
    } ints;
};

int64_t istream::get_size()
{
    int64_t pos, result;

    pos = seek(0, SO_CURRENT);
    result = seek(0, SO_END);
    seek(pos, SO_BEGINNING);

    return result;
}

//-----------------------------------------------------------------------------

void istream::read_buffer(void *buffer, int count)
{
    if (count != 0 && read(buffer, count) != count)
        throw std::exception();
}

//-----------------------------------------------------------------------------

void istream::write_buffer(const void *buffer, int count)
{
    if (count != 0 && write(buffer, count) != count)
         throw std::exception();
}

memory_stream::memory_stream(int memoryDelta)
{
    init();
}

memory_stream::memory_stream(const memory_stream& src)
{
    init();
    assign(src);
}

//-----------------------------------------------------------------------------

memory_stream::~memory_stream()
{
    clear();
}

//-----------------------------------------------------------------------------

void memory_stream::init()
{
    memory_ = NULL;
    capacity_ = 0;
    size_ = 0;
    position_ = 0;
    set_memory_delta(DEFAULT_MEMORY_DELTA);
}

//-----------------------------------------------------------------------------

memory_stream& memory_stream::operator = (const memory_stream& rhs)
{
    if (this != &rhs)
        assign(rhs);
    return *this;
}

//-----------------------------------------------------------------------------
// 描述: 读内存流
//-----------------------------------------------------------------------------
int memory_stream::read(void *buffer, int count)
{
    int result = 0;

    if (position_ >= 0 && count >= 0)
    {
        result = size_ - position_;
        if (result > 0)
        {
            if (result > count) result = count;
            memmove(buffer, memory_ + (UINT)position_, result);
            position_ += result;
        }
    }

    return result;
}

//-----------------------------------------------------------------------------
// 描述: 写内存流
//-----------------------------------------------------------------------------
int memory_stream::write(const void *buffer, int count)
{
    int result = 0;
    int pos;

    if (position_ >= 0 && count >= 0)
    {
        pos = position_ + count;
        if (pos > 0)
        {
            if (pos > size_)
            {
                if (pos > capacity_)
                    set_capacity(pos);
                size_ = pos;
            }
            memmove(memory_ + (UINT)position_, buffer, count);
            position_ = pos;
            result = count;
        }
    }

    return result;
}

//-----------------------------------------------------------------------------
// 描述: 内存流指针定位
//-----------------------------------------------------------------------------
INT64 memory_stream::seek(INT64 offset, SEEK_ORIGIN seekOrigin)
{
    switch (seekOrigin)
    {
    case SO_BEGINNING:
        position_ = (int)offset;
        break;
    case SO_CURRENT:
        position_ += (int)offset;
        break;
    case SO_END:
        position_ = size_ + (int)offset;
        break;
    }

    return position_;
}

//-----------------------------------------------------------------------------
// 描述: 设置内存流大小
//-----------------------------------------------------------------------------
void memory_stream::set_size(int64_t size)
{
    int oldPos = position_;

    set_capacity((int)size);
    size_ = (int)size;
    if (oldPos > size) seek(0, SO_END);
}

//-----------------------------------------------------------------------------
// 描述: 将其它流读入到内存流中
//-----------------------------------------------------------------------------
bool memory_stream::load(istream& stream)
{
    try
    {
        int64_t count = stream.get_size();

        stream.set_pos(0);
        set_size(count);
        if (count != 0)
            stream.read_buffer(memory_, (int)count);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// 描述: 将文件读入到内存流中
//-----------------------------------------------------------------------------
bool memory_stream::load(const std::string& fileName)
{
    file_stream fs;
    bool result = fs.open(fileName, FM_OPEN_READ | FM_SHARE_DENY_WRITE);
    if (result)
        result = load(fs);
    return result;
}

//-----------------------------------------------------------------------------
// 描述: 将内存流保存到其它流中
//-----------------------------------------------------------------------------
bool memory_stream::save(istream& stream)
{
    try
    {
        if (size_ != 0)
            stream.write_buffer(memory_, size_);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// 描述: 将内存流保存到文件中
//-----------------------------------------------------------------------------
bool memory_stream::save(const std::string& fileName)
{
    file_stream fs;
    bool result = fs.open(fileName, FM_CREATE);
    if (result)
        result = save(fs);
    return result;
}

//-----------------------------------------------------------------------------
// 描述: 清空内存流
//-----------------------------------------------------------------------------
void memory_stream::clear()
{
    set_capacity(0);
    size_ = 0;
    position_ = 0;
}

//-----------------------------------------------------------------------------

void memory_stream::assign(const memory_stream& src)
{
    set_memory_delta(src.memoryDelta_);
    set_size(src.size_);
    set_pos(src.position_);
    if (size_ > 0)
        memmove(memory_, src.memory_, size_);
}

//-----------------------------------------------------------------------------

void memory_stream::set_memory_delta(int newMemoryDelta)
{
    if (newMemoryDelta != DEFAULT_MEMORY_DELTA)
    {
        if (newMemoryDelta < MIN_MEMORY_DELTA)
            newMemoryDelta = MIN_MEMORY_DELTA;

        // 保证 newMemoryDelta 是2的N次方
        for (int i = sizeof(int) * 8 - 1; i >= 0; i--)
        if (((1 << i) & newMemoryDelta) != 0)
        {
            newMemoryDelta &= (1 << i);
            break;
        }
    }

    memoryDelta_ = newMemoryDelta;
}

//-----------------------------------------------------------------------------

void memory_stream::set_pointer(char *memory, int size)
{
    memory_ = memory;
    size_ = size;
}

//-----------------------------------------------------------------------------

void memory_stream::set_capacity(int newCapacity)
{
    set_pointer(realloc(newCapacity), size_);
    capacity_ = newCapacity;
}

//-----------------------------------------------------------------------------

char* memory_stream::realloc(int& newCapacity)
{
    char* result;

    if (newCapacity > 0 && newCapacity != size_)
        newCapacity = (newCapacity + (memoryDelta_ - 1)) & ~(memoryDelta_ - 1);

    result = memory_;
    if (newCapacity != capacity_)
    {
        if (newCapacity == 0)
        {
            free(memory_);
            result = NULL;
        }
        else
        {
            if (capacity_ == 0)
                result = (char*)malloc(newCapacity);
            else
                result = (char*)::realloc(memory_, newCapacity);

            if (!result)
                throw std::exception();
        }
    }

    return result;
}



file_stream::file_stream()
{
    init();
}

//-----------------------------------------------------------------------------
// 描述: 构造函数
// 参数:
//   fileName   - 文件名
//   openMode   - 文件流打开方式
//   rights     - 文件存取权限
//-----------------------------------------------------------------------------
file_stream::file_stream(const std::string& fileName, uint32_t openMode, uint32_t rights)
{
    init();

    if (!open(fileName, openMode, rights))
        throw std::exception();
}

//-----------------------------------------------------------------------------
// 描述: 析构函数
//-----------------------------------------------------------------------------
file_stream::~file_stream()
{
    close();
}

//-----------------------------------------------------------------------------
// 描述: 打开文件
// 参数:
//   fileName - 文件名
//   openMode   - 文件流打开方式
//   rights     - 文件存取权限
//   exception  - 如果发生异常，则传回该异常
//-----------------------------------------------------------------------------
bool file_stream::open(const std::string& fileName, uint32_t openMode, uint32_t rights)
{
    close();

    if (openMode == FM_CREATE)
        handle_ = file_create(fileName, rights);
    else
        handle_ = file_open(fileName, openMode);

    bool result = (handle_ != INVALID_HANDLE_VALUE);

    if (!result )
    {
        
    }

    fileName_ = fileName;
    return result;
}

//-----------------------------------------------------------------------------
// 描述: 关闭文件
//-----------------------------------------------------------------------------
void file_stream::close()
{
    if (handle_ != INVALID_HANDLE_VALUE)
    {
        file_close(handle_);
        handle_ = INVALID_HANDLE_VALUE;
    }
    fileName_.clear();
}

//-----------------------------------------------------------------------------
// 描述: 读文件流
//-----------------------------------------------------------------------------
int file_stream::read(void *buffer, int count)
{
    int result;

    result = file_read(handle_, buffer, count);
    if (result == -1) result = 0;

    return result;
}

//-----------------------------------------------------------------------------
// 描述: 写文件流
//-----------------------------------------------------------------------------
int file_stream::write(const void *buffer, int count)
{
    int result;

    result = file_write(handle_, buffer, count);
    if (result == -1) result = 0;

    return result;
}

//-----------------------------------------------------------------------------
// 描述: 文件流指针定位
//-----------------------------------------------------------------------------
int64_t file_stream::seek(int64_t offset, SEEK_ORIGIN seekOrigin)
{
    return file_seek(handle_, offset, seekOrigin);
}

//-----------------------------------------------------------------------------
// 描述: 设置文件流大小
//-----------------------------------------------------------------------------
void file_stream::set_size(int64_t size)
{
    bool success;
    seek(size, SO_BEGINNING);

    success = ::SetEndOfFile(handle_) != 0;

    if (!success)
        throw std::exception();
}

//-----------------------------------------------------------------------------
// 描述: 判断文件流当前是否打开状态
//-----------------------------------------------------------------------------
bool file_stream::is_open() const
{
    return (handle_ != INVALID_HANDLE_VALUE);
}

//-----------------------------------------------------------------------------
// 描述: 初始化
//-----------------------------------------------------------------------------
void file_stream::init()
{
    fileName_.clear();
    handle_ = INVALID_HANDLE_VALUE;
}

//-----------------------------------------------------------------------------
// 描述: 创建文件
//-----------------------------------------------------------------------------
HANDLE file_stream::file_create(const std::string& fileName, uint32_t rights)
{
    return ::CreateFileA(fileName.c_str(), GENERIC_READ | GENERIC_WRITE,
        0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
}

//-----------------------------------------------------------------------------
// 描述: 打开文件
//-----------------------------------------------------------------------------
HANDLE file_stream::file_open(const std::string& fileName, uint32_t openMode)
{
    uint32_t accessModes[3] = {
        GENERIC_READ,
        GENERIC_WRITE,
        GENERIC_READ | GENERIC_WRITE
    };
    uint32_t shareModes[5] = {
        0,
        0,
        FILE_SHARE_READ,
        FILE_SHARE_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE
    };

    HANDLE fileHandle = INVALID_HANDLE_VALUE;

    if ((openMode & 3) <= FM_OPEN_READ_WRITE &&
        (openMode & 0xF0) <= FM_SHARE_DENY_NONE)
        fileHandle = ::CreateFileA(fileName.c_str(), accessModes[openMode & 3],
        shareModes[(openMode & 0xF0) >> 4], NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    return fileHandle;
}

//-----------------------------------------------------------------------------
// 描述: 关闭文件
//-----------------------------------------------------------------------------
void file_stream::file_close(HANDLE handle)
{
    ::CloseHandle(handle);
}

//-----------------------------------------------------------------------------
// 描述: 读文件
//-----------------------------------------------------------------------------
int file_stream::file_read(HANDLE handle, void *buffer, int count)
{
    unsigned long result;
    if (!::ReadFile(handle, buffer, count, &result, NULL))
        result = -1;
    return result;
}

//-----------------------------------------------------------------------------
// 描述: 写文件
//-----------------------------------------------------------------------------
int file_stream::file_write(HANDLE handle, const void *buffer, int count)
{
    unsigned long result;
    if (!::WriteFile(handle, buffer, count, &result, NULL))
        result = -1;
    return result;
}

//-----------------------------------------------------------------------------
// 描述: 文件指针定位
//-----------------------------------------------------------------------------
int64_t file_stream::file_seek(HANDLE handle, int64_t offset, SEEK_ORIGIN seekOrigin)
{
    int64_t result = offset;
    ((Int64Rec*)&result)->ints.lo = ::SetFilePointer(
        handle, ((Int64Rec*)&result)->ints.lo,
        (PLONG)&(((Int64Rec*)&result)->ints.hi), seekOrigin);
    if (((Int64Rec*)&result)->ints.lo == -1 && GetLastError() != 0)
        ((Int64Rec*)&result)->ints.hi = -1;
    return result;
}


}}


