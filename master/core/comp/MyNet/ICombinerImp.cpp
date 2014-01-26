#include "stdafx.h"
#include "icombiner_imp.h"


bool icombiner_imp::IsIntactPacket(const net_stream_type& stream, int& packetlen)
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