#include "StdAfx.h"
#include "ICombinerImp.h"


bool ICombinerImp::IsIntactPacket(const net_stream_type& stream, int& packetlen)
{
//   if (*(UINT*)buff == len) {
//     return true;
//   }

  packetlen = 10;
  if (stream.size() >= 10) {
    return true;
  }

  return false;
}