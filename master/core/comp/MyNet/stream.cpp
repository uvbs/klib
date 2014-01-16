#include "StdAfx.h"
#include "stream.h"

namespace net {

istream::istream(void)
{
}


istream::~istream(void)
{
}

//------------------------------------------------------------------------
// mem_stream
int mem_stream::read(void *buffer, int count)
{
    buff_.copy((char*)buffer, 0, count);
    return 0;
}



//------------------------------------------------------------------------
// 


}