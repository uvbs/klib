/*******************************************************************************
*
* FileName : test_for_string.cpp
* Comment  : 自定义字符串类String的应用
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-5-31 3:34
*
*******************************************************************************/

#include <iostream>
#include "String.h"

using namespace std;

String f(String a, String b)
{
	try {
	a[2]='x';
	char c=b[3];
	cout<<"in f: "<<a<<' '<<b<<' '<<c<<'\n';
	}
	catch (String::Range) {
		cout<<"error: the subscript is out of range!\n";
	}
	return b;
}

int main()
{
	String x,y;
	cout<<"Please enter two strings:\n";
	cin>>x>>y;
	cout<<"input: "<<x<<' '<<y<<'\n';
	String z=x;
	y=f(x,y);
	//通过赋值传递的参数在函数中的操作不应改变它们的值
	if (x!=z) cout<<"x corrupted!\n";
	x[0]='!';
	if (x == z) cout<<"write failed!\n";
	cout<<"exit: "<<x<<' '<<y<<' '<<z<<'\n';
	x+=y;
	cout<<x<<'\n';
	String w;
	cin>>w;
	cout<<w<<endl;
	cout<<char(w[1])<<endl;
	char* s=w.c_star();
	cout<<s<<endl;
	return 0;
}