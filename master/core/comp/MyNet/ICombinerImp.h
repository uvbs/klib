#pragma once

#include "ICombiner.h"

class ICombinerImp : public ICombiner
{
public:
  virtual bool IsIntactPacket(const net_stream_type& stream, int& packetlen) ;
};