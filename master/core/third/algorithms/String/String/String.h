/*******************************************************************************
*
* FileName : String.h
* Comment  : 仿string字符串类，提供值语义、字符读写、流I/O、用字符串文字量作为
*			 文字量、相等和拼接运算符。利用引用计数减少复制，并采用静态缓冲区
*			 技术返回c风格字符串。
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-5-31 3:33
*
*******************************************************************************/

#ifndef _F_STRING_H_INCLUDED
#define _F_STRING_H_INCLUDED

#include <iostream>

using std::ostream;
using std::istream;

//程序能处理50个长度为100的字符串
const int N_INPUT=100;
const int N_BUF=50;

class String {
private:
	//使一个实际表示能被几个具有同样值得String共享
	struct Srep {
		char *s;	//到元素的值针
		int sz;		//字符个数
		int n;		//引用计数
		Srep(int nsz, const char* p)
		{
			n=1;
			sz=nsz;
			s=new char[sz+1];
			strcpy(s,p);
		}
		~Srep() { delete[] s; }
		Srep* get_own_copy()    //需要时克隆(真正的复制)
		{
			if (n==1) return this;
			n--;
			return new Srep(sz,s);
		}
		//真正意义上的赋值
		void assign(int nsz, const char* p)
		{
			if (sz!=nsz) {
				delete[] s;
				sz=nsz;
				s=new char[sz+1];
			}
			strcpy(s,p);
		}
	private:				//防止复制
		Srep(const Srep&);
		Srep& operator=(const Srep&);
	};
	Srep* rep;

	char* get_temp_string();	  //返回静态缓冲区

public:
	class Cref{	// 引用char
		friend class String;
	private:
		String& s;
		int i;
		Cref(String& ss, int ii) : s(ss), i(ii) {}
	public:
		operator char() const { return s.read(i); }
		void operator=(char c) { s.write(i,c); }
	};

	class Range {};	  //用于异常

	//构造、析构、赋值函数
	String();
	String(const char*);
	String(const String&);
	String& operator=(const char*);
	String& operator=(const String&);
	~String();

	//检查下标越界
	void check(int i) const { if (i<0 || rep->sz<=i) throw Range(); }

	char read(int i) const { return rep->s[i]; }
	void write(int i, char c) { rep=rep->get_own_copy(); rep->s[i]=c; }

	Cref operator[](int i) { check(i); return Cref(*this,i); }
	char operator[](int i) const { check(i); return rep->s[i]; }

	int size() const { return rep->sz; }
	//返回c风格字符串
	char* c_star() { char* s=get_temp_string(); strcpy(s,rep->s); return s; }

	String& operator+=(const String&);
	String& operator+=(const char*);

	friend ostream& operator<<(ostream&, const String&);
	friend istream& operator>>(istream&, String&);

	friend bool operator==(const String& x, const char* s)
	{ return strcmp(x.rep->s, s) == 0; }

	friend bool operator==(const String& x, const String& y)
	{ return strcmp(x.rep->s, y.rep->s) == 0; }

	friend bool operator!=(const String& x, const char* s)
	{ return strcmp(x.rep->s,s) != 0; }

	friend bool operator!=(const String& x, const String& y)
	{ return strcmp(x.rep->s, y.rep->s) != 0; }
};

//设置并返回静态缓冲区
char* String::get_temp_string()
{
	static int nbuf=0;
	static char buf[N_BUF][N_INPUT+1];
	for (int i=0; i<N_BUF; ++i)
		for (int j=0; j<N_INPUT; ++j)
			buf[i][j]='\0';
	if (nbuf == N_BUF) nbuf=0;
	return buf[nbuf++];
}

String::String()
{
	rep=new Srep(0,"");
}

String::String(const String& x)
{
	x.rep->n++;
	rep=x.rep;	  //通过“共享”实现复制
}

String::~String()
{
	if (--rep->n == 0) delete rep;
}

String& String::operator=(const String& x)
{
	x.rep->n++;
	if (--rep->n == 0) delete rep;
	rep=x.rep;		//通过“共享”实现赋值
	return *this;
}

String::String(const char* s)
{
	rep=new Srep(strlen(s),s);
}

String& String::operator=(const char* s)
{
	if (rep->n == 1)
		rep->assign(strlen(s),s);
	else {
		rep->n--;
		rep=new Srep(strlen(s),s);
	}
	return *this;
}

String& String::operator+=(const String& y)
{
	int nn=rep->sz+y.rep->sz;
	char* p=new char[nn+1];
	int k1=0;
	for (; k1<rep->sz; ++k1)
		p[k1]=rep->s[k1];
	for (int k2=0; k2<y.rep->sz; ++k2, ++k1)
		p[k1]=y.rep->s[k2];
	p[k1]='\0';
	rep=rep->get_own_copy();
	rep->assign(nn,p);
	return *this;
}

String& String::operator+=(const char* s)
{
	int nn=rep->sz+strlen(s);
	char* p=new char[nn+1];
	int k1=0;
	for (; k1<rep->sz; ++k1)
		p[k1]=rep->s[k1];
	for (int k2=0; k2<strlen(s); ++k2, ++k1)
		p[k1]=s[k2];
	p[k1]='\0';
	rep=rep->get_own_copy();
	rep->assign(nn,p);
	return *this;
}

ostream& operator<<(ostream& output, const String& y)
{
	output<<y.rep->s;
	return output;
}

istream& operator>>(istream& input, String& y)
{
	char* s_temp=new char[N_INPUT+1];
	input>>s_temp;
	y.rep=y.rep->get_own_copy();
	y.rep->assign(strlen(s_temp),s_temp);
	return input;
}

#endif