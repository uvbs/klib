
#ifndef MYRMI_WANG_SHU_DONG_2008_01_08
#define MYRMI_WANG_SHU_DONG_2008_01_08


#pragma comment(lib,"wsock32.lib ")

#include "stdio.h"
#include <string>
#include "winsock2.h"
#include <windows.h>//信号量，以及线程控制函数
#include <list>
#include <map>

//以下是各种常数定义
#define MAXBUFFERLENGTH 2048//缓冲区最大长度，即RemoteFunctionStu(远程方法的函数存根)得最大长度
#define SERVERPORT 1110     //服务器默认端口号
#define MAXPARAMETERCOUNT 10     //参数的最大个数
#define MAX_CLIENT_COUNT 10		//一个服务器端同时处理的最大客户端数目
#define MAX_FUN_LENGHT 1024		//函数id的最大长度


//以下是各种内部数据的定义
class MYIDENTIFYCLASS{};   //模板中默认的参数类型，程序据此判断所传入的参数类型以及数目
class MYVOIDCLASS{};		//函数无返回值时的默认类型。如果模板中的参数是此类型，则说明函数无返回值，一遍做其它相应处理

static MYIDENTIFYCLASS mDefaultPara; //全局变量用于初始化默认的模板参数---FunctionObject模板中需要


//函数存根   
struct RemoteFunctionStub{
	std::string mFunctionID;		//函数ID，每个函数均有一个唯一的ID标识此函数。系统据此ID判断调用哪个函数
	int mParaCount;					//函数的参数数目
	void* mReturnValue;				//返回值存放的位置
	int mReturnValueLength;			//返回值类型的长度
	int mStructLength;				//内部联合类型的长度，表示单个参数的最大长度
	void* pPara;					//参数表。所有参数以数组的形式存放与此处
} ;


//监听线程（ListenThread），以及请求处理线程（ProcessRequest）所需要的参数结构 
class RMIClientBase;		//客户端端基类。以下有定义
class RMIServerBase;		//服务器端基类。以下有定义
struct THREADPARA{
	SOCKET mSocket;							//socket链接
	RMIServerBase* pRMIServerBase;			//服务器端基类指针
};


//部分“检查错误用”宏

//FunctionObject模板中需要的宏
#define CHECKPARA(Para) \
	if(typeid(Para).name()[strlen(typeid(Para).name())]=='*'||              \
		typeid(Para).name()[strlen(typeid(Para).name())]==')')				\
	{																		\
		strcpy(mParaException,"错误：参数中包含指针或者引用\r\n");			\
		throw mParaException;												\
		return tError;														\
	}																	

//RMIServerBase需要的宏
#define TEST_RETURE_VALUE()\
			if(nRes<0)\
			{\
				WaitForSingleObject(mThreadPara.pRMIServerBase->mSemaphoreThreadList,5000);\
				printf("%d线程退出\r\n",mThreadPara.pRMIServerBase->mClientThreadMap[mThreadPara.mSocket]);\
				mThreadPara.pRMIServerBase->mClientThreadMap.erase(mThreadPara.mSocket);\
				mThreadPara.pRMIServerBase->mClientCount--;\
				ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);\
				closesocket(mThreadPara.mSocket);\
				ExitThread(1);\
				return 0;\
			}




//服务器端需要的模板类
//参数解析模板
template<
typename ReturnValue, 
typename P1 =MYIDENTIFYCLASS, 
typename P2 =MYIDENTIFYCLASS, 
typename P3 =MYIDENTIFYCLASS, 
typename P4 =MYIDENTIFYCLASS, 
typename P5 =MYIDENTIFYCLASS, 
typename P6 =MYIDENTIFYCLASS, 
typename P7 =MYIDENTIFYCLASS, 
typename P8 =MYIDENTIFYCLASS, 
typename P9 =MYIDENTIFYCLASS>

class ParaListAnalyser{
public:
	ParaListAnalyser(void* tp)
	{///构造函数里应该判断所传入的参数是否包含指针或者引用
		pValue=tp;
	};
	std::string GetReturnValueType(){
		std::string tstr;
		tstr=typeid(ReturnValue).name();

	}
	P1 GetP1()
	{
		return ((UNIONTYPE*)pValue)[1].a1;
	}
	P2 GetP2()
	{
		return ((UNIONTYPE*)pValue)[2].a2;
	}
	P3 GetP3()
	{
		return ((UNIONTYPE*)pValue)[3].a3;
	}
	P4 GetP4()
	{
		return ((UNIONTYPE*)pValue)[4].a4;;
	}
	P5 GetP5()
	{
		return ((UNIONTYPE*)pValue)[5].a5;
	}
	P6 GetP6()
	{
		return ((UNIONTYPE*)pValue)[6].a6;
	}
	P7 GetP7()
	{
		return ((UNIONTYPE*)pValue)[7].a7;
	}
	P8 GetP8()
	{
		return ((UNIONTYPE*)pValue)[8].a8;
	}
	P9 GetP9()
	{
		return ((UNIONTYPE*)pValue)[9].a9;
	}
private:

	union UNIONTYPE{
		ReturnValue mReturnValue;
		P1 a1;
		P2 a2;
		P3 a3;
		P4 a4;
		P5 a5;
		P6 a6;
		P7 a7;
		P8 a8;
		P9 a9;
	};
//	UNIONTYPE mTypeList[MAXPARAMETERCOUNT];
	////上面为类型判断所用到的变量
	void* pValue;
};



//以下是clientBase所需要的模板支持

template<
typename ReturnValue, 
typename P1 =MYIDENTIFYCLASS, 
typename P2 =MYIDENTIFYCLASS, 
typename P3 =MYIDENTIFYCLASS, 
typename P4 =MYIDENTIFYCLASS, 
typename P5 =MYIDENTIFYCLASS, 
typename P6 =MYIDENTIFYCLASS, 
typename P7 =MYIDENTIFYCLASS, 
typename P8 =MYIDENTIFYCLASS, 
typename P9 =MYIDENTIFYCLASS>

class FunctionObject{
public:
	FunctionObject()
	{///构造函数里应该判断所传入的参数是否包含指针或者引用

		char targetstr[]="class MYIDENTIFYCLASS";

		mTypeList[0].mType=typeid(ReturnValue).name();
		mTypeList[1].mType=typeid(P1).name();
		mTypeList[2].mType=typeid(P2).name();
		mTypeList[3].mType=typeid(P3).name();
		mTypeList[4].mType=typeid(P4).name();
		mTypeList[5].mType=typeid(P5).name();
		mTypeList[6].mType=typeid(P6).name();
		mTypeList[7].mType=typeid(P7).name();
		mTypeList[8].mType=typeid(P8).name();
		mTypeList[9].mType=typeid(P9).name();

///计算参数实例化数目
		for(mParaCount=0;mParaCount<MAXPARAMETERCOUNT&&
			!(strcmp(mTypeList[mParaCount].mType.c_str(),targetstr)==0);mParaCount++){}
		

	};
	ReturnValue operator() (const char* tRomateFunction,RMIClientBase* pClient ,P1 p1=mDefaultPara,P2 p2=mDefaultPara,P3 p3=mDefaultPara
		,P4 p4=mDefaultPara,P5 p5=mDefaultPara,P6 p6=mDefaultPara
		,P7 p7=mDefaultPara,P8 p8=mDefaultPara,P9 p9=mDefaultPara)
	{//暂不处理指针！
		
		ReturnValue r;//临时变量，供返回值用
		ReturnValue tError;//当返回错误是返回此值
		char mParaException[200];
		memset(mParaException,0,200);
	//首先判断所给参数中是否含有指针，如果有则抛出异常
		CHECKPARA(p1);
		CHECKPARA(p2);
		CHECKPARA(p3);
		CHECKPARA(p4);
		CHECKPARA(p5);
		CHECKPARA(p6);
		CHECKPARA(p7);
		CHECKPARA(p8);
		CHECKPARA(p9);

		
		mUnionList[1].a1=p1;
		mUnionList[2].a2=p2;
		mUnionList[3].a3=p3;
		mUnionList[4].a4=p4;
		mUnionList[5].a5=p5;
		mUnionList[6].a6=p6;
		mUnionList[7].a7=p7;
		mUnionList[8].a8=p8;
		mUnionList[9].a9=p9;
		//下面封装调用stub，然后调用发送函数将此stub发送出去
		mRFStub.mFunctionID=tRomateFunction;
		mRFStub.mReturnValueLength=sizeof(ReturnValue);
		if(strcmp(typeid(ReturnValue).name(),typeid(MYVOIDCLASS).name())==0)
		{//无返回值
			mRFStub.mReturnValue=NULL;
		}
		else
		{//有返回值
			mRFStub.mReturnValue=(void*)&r;
		}
		mRFStub.mStructLength=sizeof(UNIONTYPE)*(mParaCount-1);
		mRFStub.pPara=(void*)mUnionList;
		mRFStub.mParaCount=mParaCount;
		//
		if(pClient->CallRemoteFunction(&mRFStub))
			return r;
		else
			return tError;
	}
private:
	int mParaCount;
//内部结构。定义通用参数类型
	union UNIONTYPE{
		ReturnValue mReturnValue;
		P1 a1;
		P2 a2;
		P3 a3;
		P4 a4;
		P5 a5;
		P6 a6;
		P7 a7;
		P8 a8;
		P9 a9;
	};
	struct TYPEINFOR{
		UNIONTYPE mUnionType;
		std::string mType;
	};
	TYPEINFOR mTypeList[MAXPARAMETERCOUNT];
	UNIONTYPE mUnionList[MAXPARAMETERCOUNT];
	////上面为类型判断所用到的变量
	RemoteFunctionStub mRFStub;

};


//以下是通讯过程中所需要的基本类型的定义

//服务器端基类    负责基本的socket通讯工作
//此类在接到stop命令后，简单的调用TerminateThread函数将所有客户端的请求线程关闭，此欠考虑，
// 应选择一种切实有效的机制避免在客户端调用函数时正确结束，或者如果不接受则向用户传递一个消息
class RMIServerBase
{
public:
	RMIServerBase();			//
	~RMIServerBase();			//

	bool Listen(int tPort);//开始监听。端口由tPort指定
		
	bool SendResponse(void* pV,int L,SOCKET tSocket);//向tSocket发送由pV指定的长度是L的数据	

	//虚函数 。以便子类实现。调用本地方法。pFunID。函数ID。pParaList。参数表。tParaListLengt。参数列表的长度
	virtual	void CallLocalFunction(const char* pFuncID, void* pParaList,int tParaListLenght,SOCKET tSocket)=0;
	bool Stop();//停止监听
private:
	
	static DWORD WINAPI ProcessRequest(LPVOID pPara);//接收到一个请求后负责处理请求的线程
	
	static DWORD WINAPI ListenThread(LPVOID pPara);//监听线程;


private:
	bool mConnected;//标志当前是否处于监听状态
	int mServerPort;//服务端口
	
	WORD wVersionRequested;//winsocket版本 = MAKEWORD(1,1);
	WSADATA wsaData;//winsocket描述信息
	int nRet;//
	SOCKET	mLocalSocket;	//本地监听socket
	sockaddr_in mLocalAddr;//本地地址
	
	int mClientCount;//客户端请求数目
	HANDLE mSemaphoreThreadList;//信号量：互斥访问“请求处理线程列表：mClientThreadMap”
	HANDLE mSemaphoreConnect;//信号量:互斥访问：mClientCount;
	HANDLE mListenThreadHandle;//监听线程的线程句柄
	std::map<SOCKET,HANDLE> mClientThreadMap; //请求处理线程列表。保存所有正在处理请求的线程handle
	THREADPARA ThreadParaListenThread;//
	THREADPARA ThreadParaProcessRequest;//
	HANDLE hThreadRequest[MAX_CLIENT_COUNT];    //用来保存处理每个客户端请求的线程句柄
	///int 
public:
	int nError;//错误代码
	
};	


//客户端基类
class RMIClientBase
{
public:
	RMIClientBase();
	~RMIClientBase();
	bool Connect(char* tRomateIP,int tRomatePort);
	void StopConnect();
	bool IsAvailable();
	bool CallRemoteFunction(RemoteFunctionStub* tRFStub);
private:
	bool SendMessage(void* pValue,int tSendLength);
	bool mConnected;
	int mPort;//服务器端口
	std::string mRemoteIP;
	
	WORD wVersionRequested;
	WSADATA wsaData;
	int nRet,nError;
	sockaddr_in mRemoteAddr;
	
	SOCKET mRemoteSocket;
	HANDLE mHandleStopEvent;
	HANDLE mHandleSendMessageEvent;
	HANDLE mHandleListenThread;
	
};


#endif