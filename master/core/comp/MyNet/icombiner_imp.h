#pragma once

#include "icombiner.h"

class icombiner_imp : public icombiner
{
public:
  virtual bool IsIntactPacket(const net_stream_type& stream, int& packetlen) ;
};