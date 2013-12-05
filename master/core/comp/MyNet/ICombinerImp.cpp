#include "StdAfx.h"
#include "ICombinerImp.h"


bool ICombinerImp::IsIntactPacket(const char* buff, int len, int& packetlen)
{
//   if (*(UINT*)buff == len) {
//     return true;
//   }

  packetlen = 10;
  if (len >= 10) {
    return true;
  }

  return false;
}