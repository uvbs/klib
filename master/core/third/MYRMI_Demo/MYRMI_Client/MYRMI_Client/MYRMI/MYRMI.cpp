
#include "MYRMI.h"


//此类在接到stop命令后，简单的调用TerminateThread函数将所有客户端的请求线程关闭，此欠考虑，
// 应选择一种切实有效的机制避免在客户端调用函数时正确结束，或者如果不接受则向用户传递一个消息
RMIServerBase::RMIServerBase()
{
	mConnected=false;
	mServerPort=SERVERPORT;
	mClientCount=0;
	mSemaphoreThreadList=CreateSemaphore(NULL,1,10,NULL);
	mSemaphoreConnect=CreateSemaphore(NULL,1,10,NULL);
	mListenThreadHandle = NULL ;
	for( int i = 0 ; i< MAX_CLIENT_COUNT ; i++)
	{
		hThreadRequest[i] = NULL;
	}
}
RMIServerBase::~RMIServerBase()
{
	
}
bool RMIServerBase::SendResponse(void* pV,int L,SOCKET tSocket){
	if(send(tSocket,(char*)pV,L,0)==L)
		return true;
	else
		return false;
	
}
bool RMIServerBase::Listen(int tPort)
{
	mServerPort=tPort;
	if(mConnected)
	{//判断当前是否处于已链接状态
		return false;
	}
	wVersionRequested= MAKEWORD(1,1);
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet)
	{//winsocket初始化错误
		WSACleanup();
		return false;
	}
	
	//
	// Check WinSock version
	//
	if (wsaData.wVersion != wVersionRequested)
	{//winsock版本错误
		WSACleanup();
		return false;
	}
	
	mLocalSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mLocalSocket == INVALID_SOCKET)
	{//生成soket结构体错误
		return false;
	}
	
	//下面创建本地监听
	mLocalAddr.sin_family=AF_INET;
	mLocalAddr.sin_port=htons(mServerPort);
	mLocalAddr.sin_addr.S_un.S_addr=INADDR_ANY;
	if(bind(mLocalSocket,(LPSOCKADDR)&mLocalAddr,sizeof(mLocalAddr))==SOCKET_ERROR)
	{//监听绑定错误
		nError=WSAGetLastError();
		return false;
	}
	if(listen(mLocalSocket,SOMAXCONN)!=0)
	{
		nError=WSAGetLastError();
		return false;
	}
	
	
	//首先构造监听线程的参数结构
	//THREADPARA tThreadPara ;
	//tThreadPara.mSocket = mLocalSocket;
	//tThreadPara.pRMIServerBase = this;
	ThreadParaListenThread.mSocket=mLocalSocket;
	ThreadParaListenThread.pRMIServerBase=this;
	
	
	mListenThreadHandle=CreateThread(NULL,0,ListenThread,(LPVOID )&ThreadParaListenThread,0,NULL);
	
	if(mListenThreadHandle==NULL)
	{
		nError=GetLastError();
		return false;
	}
	
	//最后设置运行状态 
	mConnected=true;
	return true;
}

bool RMIServerBase::Stop()
{//有可能结束不成功
	if(WaitForSingleObject(mSemaphoreConnect,5000)==WAIT_TIMEOUT)
		return false;
	if(mConnected)
	{//正在运行，则将线程列表中的所有线程全部kill掉。

		mConnected =  false ;

		ReleaseSemaphore(mSemaphoreConnect,1,NULL);
	
		//关闭监听线程所监听的socket
		//关闭此socket后，监听线程自动退出
		closesocket(mLocalSocket);
		
		mListenThreadHandle=NULL;


		//下面关闭其它的处理线程

		if(WaitForSingleObject(mSemaphoreThreadList,5000)==WAIT_TIMEOUT)
		{
			return false;
		}
		
		std::map<SOCKET,HANDLE>::iterator mapIterator;
		
		//关闭所有打开的socket
		for(mapIterator=mClientThreadMap.begin();mapIterator!=mClientThreadMap.end();mapIterator++)
		{
			closesocket(mapIterator->first);//首先关闭客户端线程所持有的socket
// 			if(WaitForSingleObject(mapIterator->second,3000)==WAIT_TIMEOUT)//等待线程结束
// 			{
// 				
// 				TerminateThread(mapIterator->second,0);//超时，强行结束线程
// 				printf("调用TerminateThread关闭线程！\r\n");
// 			}
			
		}
		ReleaseSemaphore(mSemaphoreThreadList,1,NULL);//释放信号量
		
		WSACleanup();
		return true;
		
	}
	else{
		//没有处于运行状态
		ReleaseSemaphore(mSemaphoreConnect,1,NULL);
		WSACleanup();
		return true;
	}
}

DWORD WINAPI RMIServerBase::ProcessRequest(LPVOID pPara)
{
	
	char pFunID[MAX_FUN_LENGHT];//远程方法ID
	int mParaCount;//所调用的方法参数数目
	int mUnionLength;//每个参数“单元”的长度
	int nRes;//socket接受函数的返回字节数。
	int addrL=sizeof(sockaddr);//socket调用时需要的地址类型长度
	THREADPARA mThreadPara=*((THREADPARA*)pPara);//获取线程所需要的各种参数
	char *pParaList;
	int mFunIDLength;
	
	//接收所要调用的函数ID
	while(true)//循环接受-处理请求
	{
		///首先接受函数ID的长度
		nRes=recv(mThreadPara.mSocket,(char*)&mFunIDLength,sizeof(int),0);
		TEST_RETURE_VALUE()
					
		memset(pFunID,0,mFunIDLength+1);
		//接受函数ID
		if(mFunIDLength>MAX_FUN_LENGHT)
		{//函数ID超过规定限制
			
			WaitForSingleObject(mThreadPara.pRMIServerBase->mSemaphoreThreadList,5000);
			printf("%d线程退出\r\n",mThreadPara.pRMIServerBase->mClientThreadMap[mThreadPara.mSocket]);
			mThreadPara.pRMIServerBase->mClientThreadMap.erase(mThreadPara.mSocket);
			mThreadPara.pRMIServerBase->mClientCount--;
			ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);
			closesocket(mThreadPara.mSocket);
			ExitThread(1);
			return 0;
		}
		nRes=recv(mThreadPara.mSocket,pFunID,mFunIDLength,0);
		TEST_RETURE_VALUE()
			pFunID[nRes]='\0';
		
		
		
		//下面接受参数数目
		nRes=::recv(mThreadPara.mSocket,(char*)&mParaCount,sizeof(mParaCount),0);
		TEST_RETURE_VALUE()
			
			
			
		//下面接受“联合”类型长度
		nRes=::recv(mThreadPara.mSocket,(char*)&mUnionLength,sizeof(mUnionLength),0);
		TEST_RETURE_VALUE()
			
			
		//下面接受参数表
		pParaList=new char[mParaCount*mUnionLength];
		memset(pParaList,0,mParaCount*mUnionLength);
		if(mParaCount*mUnionLength!=0)
		{//只有有参数的时候才接受
			nRes=::recv(mThreadPara.mSocket,pParaList,mParaCount*mUnionLength,0);
			if(nRes<0)
			{
				WaitForSingleObject(mThreadPara.pRMIServerBase->mSemaphoreThreadList,5000);
				printf("%d线程退出\r\n",mThreadPara.pRMIServerBase->mClientThreadMap[mThreadPara.mSocket]);
				mThreadPara.pRMIServerBase->mClientThreadMap.erase(mThreadPara.mSocket);
				mThreadPara.pRMIServerBase->mClientCount--;
				ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);
				closesocket(mThreadPara.mSocket);

				//此处与前（TEST_RETURE_VALUE）不同，多了一项“释放存储空间”
				delete pParaList;

				ExitThread(1);
				return 0;
			}
		}
		
		//调用本地方法
		mThreadPara.pRMIServerBase->CallLocalFunction(pFunID,(void*)pParaList,mUnionLength,mThreadPara.mSocket);
		delete pParaList;
	}

	WaitForSingleObject(mThreadPara.pRMIServerBase->mSemaphoreThreadList,5000);

	mThreadPara.pRMIServerBase->mClientThreadMap.erase(mThreadPara.mSocket);
	mThreadPara.pRMIServerBase->mClientCount--;

	ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);

	closesocket(mThreadPara.mSocket);

	ExitThread(1);

	return 0;
}

DWORD WINAPI RMIServerBase::ListenThread(LPVOID pPara)//监听线程
{ 
	
	
	THREADPARA mThreadPara=*((THREADPARA*)pPara);//获取线程所需要的各种参数
	sockaddr_in tRemoteAddr;
	SOCKET tNewSocket;
	int nAddrLength=sizeof(sockaddr_in);

	while(true)
	{
		
		tNewSocket=accept(mThreadPara.mSocket,(sockaddr*)&tRemoteAddr,&nAddrLength);

		WaitForSingleObject(mThreadPara.pRMIServerBase->mSemaphoreThreadList,5000);	
		
		if(tNewSocket==INVALID_SOCKET)
		{
	
			if ( ! mThreadPara.pRMIServerBase->mConnected )
			{//“正在连接”标志被重置，程序被人为退出
				mThreadPara.pRMIServerBase->mListenThreadHandle  = NULL;
				ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);
				return 0 ;
			}
			else
			{
				ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);
				continue;
			}
		}
		
		//判断链接数目是否超过最大连接数目
		if(mThreadPara.pRMIServerBase->mClientCount>=MAX_CLIENT_COUNT)
		{
			closesocket(tNewSocket);
			ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);
			continue;
		}
	
		
		//没有超过最大数目限制。启动一个处理线程处理接受到的请求
		//先查找一个可用的线程句柄，用于保存即将生成的请求处理线程
		
// 		for ( int i = 0 ; i < MAX_CLIENT_COUNT ; i ++ )
// 		{
// 			if ( mThreadPara.pRMIServerBase->hThreadRequest[i] != NULL )
// 			{
// 				break;
// 			}
// 		}
// 
// 		if ( i >= MAX_CLIENT_COUNT )
// 		{//查找可用的句柄失败
// 			closesocket(tNewSocket);
// 			ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);
// 			continue;
// 
// 		}

		mThreadPara.pRMIServerBase->ThreadParaProcessRequest.mSocket=tNewSocket;

		mThreadPara.pRMIServerBase->ThreadParaProcessRequest.pRMIServerBase = mThreadPara.pRMIServerBase;
		

		HANDLE tHandle=CreateThread(NULL,0,ProcessRequest,(LPVOID )&mThreadPara.pRMIServerBase->ThreadParaProcessRequest,0,NULL);

		if(tHandle==NULL)
		{
			mThreadPara.pRMIServerBase->nError = GetLastError();
			continue;
		}
		
		
		//增加线程数目 

		mThreadPara.pRMIServerBase->mClientThreadMap.insert(std::pair<SOCKET,HANDLE>(tNewSocket,tHandle));
		mThreadPara.pRMIServerBase->mClientCount++;
		ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);
		
	}
	
	return 0;
}





//////////////////////////////////////////////////////////////////////////
//下面是RMIClientBase的具体实现

RMIClientBase::RMIClientBase(){
	mConnected=false;
}
RMIClientBase::~RMIClientBase()
{
	StopConnect();
}
bool RMIClientBase::Connect(char* tRomateIP,int tRomatePort)
{
	mRemoteIP=tRomateIP;
	mPort=tRomatePort;
	if(mConnected)
	{//判断当前是否处于已链接状态
		return false;
	}
	wVersionRequested= MAKEWORD(1,1);
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet)
	{//winsocket初始化错误
		WSACleanup();
		return false;
	}
	
	//
	// Check WinSock version
	//
	if (wsaData.wVersion != wVersionRequested)
	{//winsock版本错误
		WSACleanup();
		return false;
	}
	
	mRemoteSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mRemoteSocket == INVALID_SOCKET)
	{//生成soket结构体错误
		WSACleanup();
		return false;
	}
	
	//设置Socket信息
	BOOL sockval;
	sockval = TRUE;

	setsockopt(mRemoteSocket, SOL_SOCKET, SO_KEEPALIVE,(char*) &sockval, sizeof(sockval));

	mRemoteAddr.sin_port = htons(tRomatePort);
	mRemoteAddr.sin_family = AF_INET;
	mRemoteAddr.sin_addr.s_addr=inet_addr(tRomateIP);
	//以上是创建服务器端的链接;
	if(connect(mRemoteSocket,(LPSOCKADDR)&mRemoteAddr,sizeof(mRemoteAddr))==SOCKET_ERROR)
	{//监听绑定错误
		WSACleanup();
		return false;
	}
	
	mConnected=true;
	return true;
};
//	static DWORD WINAPI ConnectHolder
void RMIClientBase::StopConnect()
{
	if(mConnected)
	{
		closesocket(mRemoteSocket);
	}
	
	WSACleanup();

	mConnected=false;
}
bool RMIClientBase::IsAvailable()
{
	int optValue;
	int optLength = sizeof(optValue);

	getsockopt(mRemoteSocket,SOL_SOCKET,SO_ACCEPTCONN,(char*)&optValue,&optLength) ;
	if(optValue != 1)
	{//没有建立连接

	}
	if(WSAGetLastError()==WSAECONNRESET) 
	{//连接已关闭
	
		StopConnect();
	}
	
	return mConnected;
};
bool RMIClientBase::CallRemoteFunction(RemoteFunctionStub* tRFStub)
{///此方法应该在开头判断与服务器的tcp链接仍然有效 
	
	
	///先发送RemoteFunctionStub的mFunctionID
	
	int nReCount=0;//返回字节数
	char* pReBuffer=NULL;//将返回结果保存与此处
	//发送函数ID的长度
	int tStrLength=tRFStub->mFunctionID.length();
	if(!SendMessage((void*)&tStrLength,sizeof(tStrLength)))
		return false;
	//发送函数ID
	if(!SendMessage((void*)tRFStub->mFunctionID.c_str(),tRFStub->mFunctionID.length()))
		return false;
	//发送参数数目
	if(!SendMessage((void*)&(tRFStub->mParaCount),sizeof(tRFStub->mParaCount)))
		return false;
	//发送“联合”类型长度
	if(!SendMessage((void*)&(tRFStub->mStructLength),sizeof(tRFStub->mStructLength)))
		return false;
	//发送参数表
	if(!SendMessage((void*)(tRFStub->pPara),(tRFStub->mStructLength)*(tRFStub->mParaCount)))
		return false;
	if(tRFStub->mReturnValue==NULL)
	{//无返回值
		return true;
	}
	else
	{//有返回值
		pReBuffer=new char[tRFStub->mReturnValueLength];
		memset((void*)pReBuffer,0,tRFStub->mReturnValueLength);
		int sockaddrlengh=sizeof(sockaddr);
		nReCount=recvfrom(mRemoteSocket,pReBuffer,tRFStub->mReturnValueLength,0,(sockaddr*)&mRemoteAddr,&sockaddrlengh);
		if(nReCount==tRFStub->mReturnValueLength)
		{//成功接受到
			memcpy(tRFStub->mReturnValue,pReBuffer,nReCount);
			delete pReBuffer;
			return true;
		}
		else
		{
			delete pReBuffer;
			return false;
		}
	}
};
bool RMIClientBase::SendMessage(void* pValue,int tSendLength)
{
	if(!mConnected)
		return false;
	if(send(mRemoteSocket,(char*)pValue,tSendLength,0)<tSendLength)
		return false;
	else
		return true;
};