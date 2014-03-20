
#pragma once
#ifndef _klib_tlv_archive_h_
#define _klib_tlv_archive_h_


#include "net_archive.h"


namespace klib {
namespace net {

#define length_seg_length (2)

class tlv_archive : public net_archive
{
public:
    tlv_archive(){}
    tlv_archive(char* buff, size_t buflen) {
        this->set_buff(buff, buflen);
    }

    void set_buff(char* buff, UINT buflen) 
    {
        net_buff_ = buff;

        net_archive::set_buff(buff + length_seg_length, 
            buflen - length_seg_length);
    }

    char* get_buff() {
        return net_buff_;
    }

    unsigned int get_data_len() {
        *(USHORT*) net_buff_ = net_archive::get_data_len();
        return net_archive::get_data_len() + length_seg_length;
    }

    USHORT get_seg_len()
    {
        return *(USHORT*) net_buff_;
    }

protected:
    char* net_buff_;

};




}}



#endif // _klib_tlv_archive_h_