#include <iostream>
using namespace std;

/* 最长公共子串 Longest Common Substring */

int maxlen;    /* 记录最大公共子串长度 */
int maxindex;  /* 记录最大公共子串在串1的起始位置 */
void outputLCS(char * X);  /* 输出LCS */

/* 最长公共子串 基本算法 */
int comlen(char * p, char * q)
{
	int len = 0;
	while(*p && *q && *p++ == *q++)
	{
		++len;
	}
	return len;
}

void LCS_base(char * X, int xlen, char * Y, int ylen)
{
	for(int i = 0; i < xlen; ++i)
	{
		for(int j = 0; j < ylen; ++j)
		{
			int len = comlen(&X[i],&Y[j]);
			if(len > maxlen)
			{
				maxlen = len;
				maxindex = i;
			}
		}
	}
	outputLCS(X);
}

/* 最长公共子串 DP */
int dp[30][30];

void LCS_dp(char * X, int xlen, char * Y, int ylen)
{
	maxlen = maxindex = 0;
	for(int i = 0; i < xlen; ++i)
	{
		for(int j = 0; j < ylen; ++j)
		{
			if(X[i] == Y[j])
			{
				if(i && j)
				{
					dp[i][j] = dp[i-1][j-1] + 1;
				}
				if(i == 0 || j == 0)
				{
					dp[i][j] = 1;
				}
				if(dp[i][j] > maxlen)
				{
					maxlen = dp[i][j];
					maxindex = i + 1 - maxlen;
				}
			}
		}
	}
	outputLCS(X);
}

/* 最长公共子串 后缀数组 */
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
			return len;
		}
	}
	return 0;
}

void LCS_suffix(char * X, int xlen, char * Y, int ylen)
{
	int suf_index = maxlen = maxindex = 0;

	int len_suff = xlen + ylen + 1;
	char * arr = new char [len_suff + 1];  /* 将X和Y连接到一起 */
	strcpy(arr,X);
	arr[xlen] = '#';
	strcpy(arr + xlen + 1, Y);

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
			suf_index = i;
		}
	}
	outputLCS(suff[suf_index]);
}

/* 输出LCS 
 * 在后缀数组方法中，maxindex=0
 * 因为传进来的就是后缀数组suff[]，从0打印maxlen个字符
 */
void outputLCS(char * X)
{
	if(maxlen == 0)
	{
		printf("NULL LCS\n");
		return;
	}
	printf("The len of LCS is %d\n",maxlen);

	int i = maxindex;
	while(maxlen--)
	{
		printf("%c",X[i++]);
	}
	printf("\n");
}

void main()
{
	char X[] = "aaaba";
	char Y[] = "abaa";

	/* 基本算法 */
	LCS_base(X,strlen(X),Y,strlen(Y));

	/* DP算法 */
	LCS_dp(X,strlen(X),Y,strlen(Y));

	/* 后缀数组方法 */
	LCS_suffix(X,strlen(X),Y,strlen(Y));
}