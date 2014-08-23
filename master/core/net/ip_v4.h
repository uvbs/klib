#ifndef _klib_ip_v4_h
#define _klib_ip_v4_h

#include <vector>

namespace klib {
namespace net {

class ip_v4
{
public:
    ip_v4::ip_v4() : ip_val_(0) {}
    ip_v4::ip_v4(DWORD ip) : ip_val_(ip) {}
    ip_v4::~ip_v4(void) {}

public:
    DWORD get_val () const      { return ip_val_;     }
    unsigned char* get_ip_buf() { return &ip_seg_[0]; }
    void set_val(DWORD ip)  {  ip_val_ = ip;  }

    friend bool operator<(const ip_v4 &ipLeft, const ip_v4 & ipRight)
    {
        return (ipLeft.ip_val_ < ipRight.ip_val_);
    }

    friend bool operator > (const ip_v4 &ipLeft, const ip_v4 & ipRight)
    {
        return (ipLeft.ip_val_ > ipRight.ip_val_);
    }

protected:
    //----------------------------------------------------------------------
    // ¾ùÊÇÍøÂç×Ö½ÚÐò
    union 
    {
        DWORD           ip_val_;
        unsigned char   ip_seg_[4];
    };
};

typedef std::vector<ip_v4> ipv4_list_type;


}} // namespace 

#endif
