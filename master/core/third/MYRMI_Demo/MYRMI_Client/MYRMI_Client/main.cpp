
#include "./MYRMI/MYRMI_MARCO.h"
#include <string>
#include <iostream>
using namespace std;

class Student
{
public :
	int age;
	char name[10] ;
	int sex;
};

class Teacher
{

public :
	int age;
	char name[10] ;
	int sex;
};

class Calculate{
public:
	int sum(int a,int b)
	{
		printf("int类型的sum·方法被调用\r\n");
		t1=a;
		t2=b;
		return (int)(a+b);
	}
	double sum(double a,double b)
	{
		printf("double类型的sum方法被调用\r\n");
		t1=a;
		t2=b;
		return (a+b);
	}
	int GetInput()
	{
		int a;
		printf("请输入一个整数以便传输至客户端:\r\n");
		scanf("%d",&a);
		return a;
	}

	Student  GetStudent(Teacher tt)
	{//Sudent ,Teacher 均为自定义类型
		Student  ts;
		ts.age=10;
		printf("GetStuden方法被调用\r\n teacher 的名称以及年龄为: %s  %d\r\n",tt.name,tt.age);
		strcpy(ts.name,"StudentJim");
		ts.sex=1;
		return ts;
	}
	int t1;
	int t2;
};

MY_RMI_CLIENT_CLASS_DECLARE(RMIClient)
MY_RMI_CLIENT_FUNCTION_ADD_P2(int,sum,int,int)
MY_RMI_CLIENT_FUNCTION_ADD_P2(double,sum,double,double)
MY_RMI_CLIENT_FUNCTION_ADD_P0(int,GetInput)
MY_RMI_CLIENT_FUNCTION_ADD_P1(Student,GetStudent,Teacher)
MY_RMI_CLIENT_CLASS_END()

void main()
{
	RMIClient<Calculate> ClientCalculate;
	if ( !ClientCalculate.Connect("127.0.0.1",663))
	{
		cout<<"Can't connect to Server "<<endl;
		return ;
	}
	else
	{

		cout<<"Connected to Server successfully!"<<endl;
	}
	int nResult = ClientCalculate.sum(2,33);
	cout<<"The Result from remote \"sum(2,33)\" is : "<<nResult<<endl;

	double dResult = ClientCalculate.sum(2.2,33.21);
	cout<<"The Result from remote \"sum(2.2,33.21)\" is : "<<dResult<<endl;


	Teacher t1;
	Student s1;
	t1.age = 56;
	strcpy(t1.name,"Mr.Li");
	t1.sex = 0;
	s1=ClientCalculate.GetStudent(t1);
	ClientCalculate.StopConnect();

}