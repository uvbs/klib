#pragma once

#include "ICombiner.h"

class ICombinerImp : public ICombiner
{
public:
  virtual bool IsIntactPacket(const char* buff, int len, int& packetlen) ;
};