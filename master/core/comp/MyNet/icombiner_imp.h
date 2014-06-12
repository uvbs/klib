#ifndef _klib_icombiner_imp_h
#define _klib_icombiner_imp_h

#include "icombiner.h"

class icombiner_imp : public icombiner
{
public:
  virtual bool is_intact_packet(const net_stream_type& stream, int& packetlen) ;
};

#endif
