#pragma once

#include <string>
using std::string;

// {27FD496C-2135-4930-BB1A-B7BDFCA8DF9F}
static const GUID IID_IPinying = 
{ 0x27fd496c, 0x2135, 0x4930, { 0xbb, 0x1a, 0xb7, 0xbd, 0xfc, 0xa8, 0xdf, 0x9f } };


struct IPinying
{
public:
  /*
    作用：将一串的字符转换成拼音
    chinese：中文字符串
    len:  字符串长度
    strSpell: 转换后的拼音拼写
  */
  virtual bool GetPinyin(const char* chinese, int len, string& strSpell) = 0;

  /*
    作用：获得中文的首字母
    chinese: 中文字母
    len: 中文字母的长度
    strFirstLetters: 生成的首字母
  */
	virtual bool GetFirstLetters(const char* chinese, int len, string& strFirstLetters) = 0;
};

