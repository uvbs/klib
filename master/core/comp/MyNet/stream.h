#ifndef _klib_mynet_stream_h_
#define _klib_mynet_stream_h_


#include <inttype.h>
#include <mem/mem_buffer.h>


typedef klib::mem::mem_buffer<1024*1024*60, 10*1024, FALSE>  net_mem_type;

namespace stream
{

class istream
{
public:
    istream(void);
    virtual ~istream(void);

public:
    virtual int read(void *buffer, int count) = 0;
    virtual int write(const void *buffer, int count) = 0;
    virtual int peek(void *buffer, int count) = 0;

    virtual int64_t get_size() = 0;
};

class mem_stream : public istream
{
public:
    virtual int read(void *buffer, int count) ;
    virtual int write(const void *buffer, int count) ;
    virtual int peek(void *buffer, int count) ;

    virtual int64_t get_size() ;
protected:
    net_mem_type buff_;
};

class file_stream : public istream
{
public:
    virtual int read(void *buffer, int count) ;
    virtual int write(const void *buffer, int count) ;
    virtual int peek(void *buffer, int count) ;

    virtual int64_t get_size() ;

protected:

};



}

#endif // _klib_mynet_stream_h_