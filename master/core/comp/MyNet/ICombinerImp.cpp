#include "stdafx.h"
#include "icombiner_imp.h"


bool icombiner_imp::is_intact_packet(const net_stream_type& stream, int& packetlen)
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