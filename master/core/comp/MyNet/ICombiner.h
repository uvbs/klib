#pragma once

/*  协议分包合并器(用在tcp中)  */

#include "net_conn.h"

class icombiner
{
public:
  /*
	buff: 被检查的缓冲区，判断是否能构成一个网络封包
    bufflen: 缓存区buff中有效数据长度
    packetlen: 组成封包后的长度,这个可能小于bufflen, 也可能大于bufflen
  */
  virtual bool is_intact_packet(const net_stream_type& stream, int& packetlen) = 0;
};