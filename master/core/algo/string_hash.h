#pragma once
#include "../istddef.h"
#include <string>

namespace klib {
namespace algo {

using namespace std;


#define MAXTABLELEN 1024    // 默认哈希索引表大小 
//////////////////////////////////////////////////////////////////////////  
// 哈希索引表定义  
typedef struct  _HASHTABLE
{  
	long nHashA;
	long nHashB;
	bool bExists;
} HASHTABLE, *PHASHTABLE ;

class string_hash
{
public:
	string_hash(const long ntable_length = MAXTABLELEN);
	~string_hash(void);

private:  
	unsigned long   cryptTable[0x500];      // 
	unsigned long   m_tablelength;          // 哈希索引表长度  
	HASHTABLE *     m_HashIndexTable; 

private:
	void init_crypt_table();                                               // 对哈希索引表预处理 
	unsigned long caculate_hash(const string& lpszString, unsigned long dwHashType); // 求取哈希值   

public:
	bool hash(const string& url);
	unsigned long is_hashed(const string&  url);    // 检测url是否被hash过
};


}}