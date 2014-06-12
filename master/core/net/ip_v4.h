#ifndef _klib_ip_v4_h
#define _klib_ip_v4_h

namespace klib {
namespace net {


class ip_v4
{
public:
    ip_v4::ip_v4() 
    {
        m_dwIP = 0;
    }

    ip_v4::ip_v4(DWORD ip)  
    {
        m_dwIP = ip;
    }

    ip_v4::~ip_v4(void)
    {
    }

public:
    operator DWORD () const
    {
        return m_dwIP;
    }
	    
    friend inline bool operator<(const ip_v4 &ipLeft, const ip_v4 & ipRight)
    {
        return (ipLeft.m_dwIP < ipRight.m_dwIP);
    }

    friend inline bool operator > (const ip_v4 &ipLeft, const ip_v4 & ipRight)
    {
        return (ipLeft.m_dwIP > ipRight.m_dwIP);
    }
public:
    //----------------------------------------------------------------------
    // ¾ùÊÇÍøÂç×Ö½ÚÐò
    //----------------------------------------------------------------------

    union 
    {
        DWORD           m_dwIP;
        unsigned char   m_IpSeg[4];
    };
};


}} // namespace 

#endif
