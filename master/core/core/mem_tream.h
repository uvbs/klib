#pragma once

#include <windows.h>
#include <stdint.h>

///< »º³åÇøÁ÷²Ù×÷
namespace klib {
namespace mem {


class mem_stream
{
public:
  mem_stream(void) :m_datalen(0) {}
  ~mem_stream(void) {}

public:
  size_t GetLength() { return m_datalen; }
  const char* c_str() const {return (const char*)m_pointer;}
  const void *value(size_t size){
    size_t tmp = m_datalen;
    m_datalen += size;
    return ((const char*)m_pointer + tmp);
  }
  void SetPointer(void* value){m_pointer = value;}
  void* GetPonter() const {return m_pointer;}


#define OPERATOR_IN(type) \
    mem_stream &operator<< (const type value) { \
    * ((type*)m_pointer + m_datalen) = value; \
    m_datalen += sizeof(type); \
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
    mem_stream &operator>>(type &value) { \
    value = *((const type*)m_pointer + m_datalen); \
    m_datalen += sizeof(type);\
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

protected:
  void* m_pointer;
  size_t  m_datalen;
};

class write_stream
{
public:
  write_stream(void* value):m_pointer(value){}
  ~write_stream(void){};

#define OPERATOR_IN(type) \
  write_stream &operator<< (const type value) { \
  *(type*)m_pointer = value; \
  m_pointer = (UINT8*)m_pointer + sizeof(type); \
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

    void SetPointer(void* value){m_pointer = value;}
  void* GetPonter() const {return m_pointer;}
  char* c_str() const {return (char*)m_pointer;}
private:
  void *m_pointer;
};

class read_stream
{
public:
  read_stream(const void* value):m_pointer(value){}
  ~read_stream(void){}

#define OPERATOR_OUT(type) \
  read_stream &operator>>(type &value) { \
  value = *(const type*)m_pointer; \
  m_pointer = (const UINT8*)m_pointer + sizeof(type);\
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
    void SetPointer(void* value){m_pointer = value;}
  const void* GetPointer() const {return m_pointer;}

  const char* c_str() const {return (const char*)m_pointer;}

  const void *value(size_t size){
    const void *pointer = m_pointer;
    m_pointer = (const UINT8*)m_pointer + size;
    return pointer;
  }
private:
  const void* m_pointer;
};



}}

