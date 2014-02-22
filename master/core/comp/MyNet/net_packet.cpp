#include "StdAfx.h"
#include "net_packet.h"


net_packet::net_packet(void)
{
    pConn		    = NULL;

    bf_size_        = 0;
    bf_ptr_         = NULL;
}

net_packet::~net_packet(void)
{
    if (NULL != bf_ptr_) {
        delete[] bf_ptr_;
        bf_ptr_ = NULL;
    }
}

bool net_packet::init_buff(size_t bf_size)
{
    if (bf_ptr_) 
    {
        delete[] bf_ptr_;
        bf_ptr_  = 0;
        bf_size_ = 0;
    }

    bf_ptr_ = new char[bf_size];
    if (NULL == bf_ptr_) 
    {
        return false;
    }

    bf_size_ = bf_size;
    return true;
}

char* net_packet::get_buff() 
{
    return bf_ptr_;
}