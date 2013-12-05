#include <iostream>
using namespace std;

/* 最长回文子串 LPS */

int maxlen;  // 记录最长回文子串的长度

/* 后缀数组 */
char * suff[100];

int pstrcmp(const void *p, const void *q)
{
	return strcmp(*(char**)p,*(char**)q);
}

int comlen_suff(char * p, char * q)
{
	int len = 0;
	while(*p && *q && *p++ == *q++)
	{
		++len;
		if(*p == '#' || *q == '#')
		{
			break;
		}
	}
	int count = 0;
	while(*p)
	{
		if(*p++ == '#')
		{
			++count;
			break;
		}
	}
	while(*q)
	{
		if(*q++ == '#')
		{
			++count;
			break;
		}
	}
	if(count == 1)
		return len;
	return 0;
}

void LPS_suffix(char * X, int xlen)
{
	maxlen = 0;

	int len_suff = 2 * xlen + 1;
	char * arr = new char [len_suff + 1];  /* 将X和逆序X连接到一起 */
	strcpy(arr,X);
	arr[xlen] = '#';

	char *p = X, *q = arr + len_suff;  /* 逆序复制 */
	*q = '\0';
	while(*p && (*--q = *p++));

	for(int i = 0; i < len_suff; ++i)  /* 初始化后缀数组 */
	{
		suff[i] = & arr[i];
	}

	qsort(suff, len_suff, sizeof(char *), pstrcmp);

	for(int i = 0; i < len_suff-1; ++i)
	{
		int len = comlen_suff(suff[i],suff[i+1]);
		if(len > maxlen)
		{
			maxlen = len;
		}
	}
}

/* O(n)解法 */
#define MIN(a,b) ((a) < (b) ? (a) : (b))

int maxid;        // 最长回文子串下标
int LPS_rb[100];  // i为中心的回文子串右边界下标right border
char str[100];    // 原字符串处理后的副本

void LPS_linear(char * X, int xlen)
{
	maxlen = maxid = 0;

	str[0] = '$';  // 将原串处理成所需的形式
	char *p = str;
	*(++p)++ = '#';
	while((*p++ = *X++) != '\0')
	{
		*p++ = '#';
	}
	
	for(int i = 1; str[i]; ++i)  // 计算LPS_rb的值
	{
		if(maxlen > i)          // 初始化LPS[i]
		{
			LPS_rb[i] = MIN(LPS_rb[2*maxid-i],(maxlen-i));
		}else
		{
			LPS_rb[i] = 1;
		}
		while(str[i-LPS_rb[i]] == str[i+LPS_rb[i]]) // 扩展
		{
			++LPS_rb[i];
		}
		if(LPS_rb[i]-1 > maxlen)
		{
			maxlen = LPS_rb[i]-1;
			maxid = i;
		}
	}
}

void main()
{
	char X[30];  // 设串不超过30
	/* test case
	 * aaaa
	 * abab
	 */
	while(cin.getline(X,30))
	{
		/* 后缀数组方法 */
		LPS_suffix(X,strlen(X));
		printf("%d\n", maxlen);

		/* O(n)方法 */
		LPS_linear(X,strlen(X));
		printf("%d\n", maxlen);
	}	
}