
//此文件仅包含大量的宏定义

#ifndef MYRMI_MACRO_WANG_SHU_DONG_2008_01_08
#define MYRMI_MACRO_WANG_SHU_DONG_2008_01_08


#include "MYRMI.H"


//字符串生成以及联合用的宏
#define MAKESTRING(tstr) #tstr
#define JOINSTRING2(str1,str2) MAKESTRING(str1##str2)
#define JOINSTRING3(str1,str2,str3) MAKESTRING(str1##str2##str3)
#define JOINSTRING4(str1,str2,str3,str4) MAKESTRING(str1##str2##str3##str4)
#define JOINSTRING5(str1,str2,str3,str4,str5) MAKESTRING(str1##str2##str3##str4##str5)
#define JOINSTRING6(str1,str2,str3,str4,str5,str6) MAKESTRING(str1##str2##str3##str4##str5##str6)
#define JOINSTRING7(str1,str2,str3,str4,str5,str6,str7) MAKESTRING(str1##str2##str3##str4##str5##str6##str7)
#define JOINSTRING8(str1,str2,str3,str4,str5,str6,str7,str8) MAKESTRING(str1##str2##str3##str4##str5##str6##str7##str8)
#define JOINSTRING9(str1,str2,str3,str4,str5,str6,str7,str8,str9) MAKESTRING(str1##str2##str3##str4##str5##str6##str7##str8##str9)
#define JOINSTRING10(str1,str2,str3,str4,str5,str6,str7,str8,str9,str10) MAKESTRING(str1##str2##str3##str4##str5##str6##str7##str8##str9##str10)
#define JOINSTRING11(str1,str2,str3,str4,str5,str6,str7,str8,str9,str10,str11) MAKESTRING(str1##str2##str3##str4##str5##str6##str7##str8##str9##str10##str11)
#define JOINSTRING12(str1,str2,str3,str4,str5,str6,str7,str8,str9,str10,str11,str12) MAKESTRING(str1##str2##str3##str4##str5##str6##str7##str8##str9##str10##str11##str12)


///客户端 宏声明一个类
#define MY_RMI_CLIENT_CLASS_DECLARE(client_class_name)									\
template<typename classname>													\
class client_class_name:public RMIClientBase				\
{																				\
private:																		\

//客户端宏定义添加一个方法


//先定义没有返回值的函数

#define MY_RMI_CLIENT_FUNCTION_ADD_V_P0(FunName)\
	public:\
		void FunName()\
		{\
			FunctionObject<MYVOIDCLASS>()(JOINSTRING2(void,FunName),this);\
		}	


#define MY_RMI_CLIENT_FUNCTION_ADD_V_P1(FunName,P1)\
	public:\
		void FunName(P1 p1)\
		{\
			FunctionObject<MYVOIDCLASS,P1>()(JOINSTRING3(void,FunName,P1),this,p1);\
		}	


#define MY_RMI_CLIENT_FUNCTION_ADD_V_P2(FunName,P1,P2)\
	public:\
		void FunName(P1 p1,P2 p2)\
		{\
			FunctionObject<MYVOIDCLASS,P1,P2>()(JOINSTRING4(void,FunName,P1,P2),this,p1,p2);\
		}

#define MY_RMI_CLIENT_FUNCTION_ADD_V_P3(FunName,P1,P2,P3)\
	public:															\
		void FunName(P1 p1,P2 p2,P3 p3)\
		{\
			FunctionObject<MYVOIDCLASS,P1,P2,P3>()(JOINSTRING5(void,FunName,P1,P2,P3),this,p1,p2,p3);\
		}

#define MY_RMI_CLIENT_FUNCTION_ADD_V_P4(FunName,P1,P2,P3,P4)\
	public:															\
		void FunName(P1 p1,P2 p2,P3 p3,P4 p4)\
		{\
			FunctionObject<MYVOIDCLASS,P1,P2,P3,P4>()(JOINSTRING6(void,FunName,P1,P2,P3,P4),this,p1,p2,p3,p4);\
		}

#define MY_RMI_CLIENT_FUNCTION_ADD_V_P5(FunName,P1,P2,P3,P4,P5)\
	public:															\
		void FunName(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)\
		{\
			FunctionObject<MYVOIDCLASS,P1,P2,P3,P4,P5>()(JOINSTRING7(void,FunName,P1,P2,P3,P4,P5),this,p1,p2,p3,p4,p5);\
		}

#define MY_RMI_CLIENT_FUNCTION_ADD_V_P6(FunName,P1,P2,P3,P4,P5,P6)\
	public:															\
		void FunName(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6)\
		{\
			FunctionObject<MYVOIDCLASS,P1,P2,P3,P4,P5,P6>()(JOINSTRING8(void,FunName,P1,P2,P3,P4,P5,P6),p1,p2,p3,p4,p5,p6);\
		}

#define MY_RMI_CLIENT_FUNCTION_ADD_V_P7(FunName,P1,P2,P3,P4,P5,P6,P7)\
	public:															\
		void FunName(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7)\
		{\
			FunctionObject<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7>()(JOINSTRING9(void,FunName,P1,P2,P3,P4,P5,P6,P7),this,p1,p2,p3,p4,p5,p6,p7);\
		}

#define MY_RMI_CLIENT_FUNCTION_ADD_V_P8(FunName,P1,P2,P3,P4,P5,P6,P7,P8)\
	public:															\
		void FunName(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8)\
		{\
			FunctionObject<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8>()(JOINSTRING10(void,FunName,P1,P2,P3,P4,P5,P6,P7,P8),this,p1,p2,p3,p4,p5,p6,p7,p8);\
		}

#define MY_RMI_CLIENT_FUNCTION_ADD_V_P9(FunName,P1,P2,P3,P4,P5,P6,P7,P8,P9)\
	public:															\
		void FunName(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9)\
		{\
			FunctionObject<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8,P9>()(JOINSTRING11(void,FunName,P1,P2,P3,P4,P5,P6,P7,P8,P9),this,p1,p2,p3,p4,p5,p6,p7,p8,p9);\
		}

//以下是有返回值的函数定义
#define MY_RMI_CLIENT_FUNCTION_ADD_P0(R,FunName)\
	public:\
		R FunName()\
		{\
			return 	FunctionObject<R>()(JOINSTRING2(R,FunName),this);\
		}	


#define MY_RMI_CLIENT_FUNCTION_ADD_P1(R,FunName,P1)\
	public:\
		R FunName(P1 p1)\
		{\
			return 	FunctionObject<R,P1>()(JOINSTRING3(R,FunName,P1),this,p1);\
		}	

#define MY_RMI_CLIENT_FUNCTION_ADD_P2(R,FunName,P1,P2)\
	public:\
		R FunName(P1 p1,P2 p2)\
		{\
			return 	FunctionObject<R,P1,P2>()(JOINSTRING4(R,FunName,P1,P2),this,p1,p2);\
		}

#define MY_RMI_CLIENT_FUNCTION_ADD_P3(R,FunName,P1,P2,P3)\
	public:															\
		R FunName(P1 p1,P2 p2,P3 p3)\
		{\
			return 	FunctionObject<R,P1,P2,P3>()(JOINSTRING5(R,FunName,P1,P2,P3),this,p1,p2,p3);\
		}

#define MY_RMI_CLIENT_FUNCTION_ADD_P4(R,FunName,P1,P2,P3,P4)\
	public:															\
		R FunName(P1 p1,P2 p2,P3 p3,P4 p4)\
		{\
			return 	FunctionObject<R,P1,P2,P3,P4>()(JOINSTRING6(R,FunName,P1,P2,P3,P4),this,p1,p2,p3,p4);\
		}

#define MY_RMI_CLIENT_FUNCTION_ADD_P5(R,FunName,P1,P2,P3,P4,P5)\
	public:															\
		R FunName(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)\
		{\
			return 	FunctionObject<R,P1,P2,P3,P4,P5>()(JOINSTRING7(R,FunName,P1,P2,P3,P4,P5),this,p1,p2,p3,p4,p5);\
		}

#define MY_RMI_CLIENT_FUNCTION_ADD_P6(R,FunName,P1,P2,P3,P4,P5,P6)\
	public:															\
		R FunName(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6)\
		{\
			return 	FunctionObject<R,P1,P2,P3,P4,P5,P6>()(JOINSTRING8(R,FunName,P1,P2,P3,P4,P5,P6)this,p1,p2,p3,p4,p5,p6);\
		}

#define MY_RMI_CLIENT_FUNCTION_ADD_P7(R,FunName,P1,P2,P3,P4,P5,P6,P7)\
	public:															\
		R FunName(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7)\
		{\
			return 	FunctionObject<R,P1,P2,P3,P4,P5,P6,P7>()(JOINSTRING9(R,FunName,P1,P2,P3,P4,P5,P6,P7),this,p1,p2,p3,p4,p5,p6,p7);\
		}

#define MY_RMI_CLIENT_FUNCTION_ADD_P8(R,FunName,P1,P2,P3,P4,P5,P6,P7,P8)\
	public:															\
		R FunName(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8)\
		{\
			return 	FunctionObject<R,P1,P2,P3,P4,P5,P6,P7,P8>()(JOINSTRING10(R,FunName,P1,P2,P3,P4,P5,P6,P7,P8),this,p1,p2,p3,p4,p5,p6,p7,p8);\
		}

#define MY_RMI_CLIENT_FUNCTION_ADD_P9(R,FunName,P1,P2,P3,P4,P5,P6,P7,P8,P9)\
	public:															\
		R FunName(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9)\
		{\
			return 	FunctionObject<R,P1,P2,P3,P4,P5,P6,P7,P8,P9>()(JOINSTRING11(R,FunName,P1,P2,P3,P4,P5,P6,P7,P8,P9),this,p1,p2,p3,p4,p5,p6,p7,p8,p9);\
		}

#define MY_RMI_CLIENT_CLASS_END()     };


//服务器端宏声明
#define MY_RMI_SERVER_CLASS_DECLARE(server_class_name)			\
template <typename TServer>										\
	class server_class_name	:public RMIServerBase				\
{																\
public:															\
	server_class_name()											\
	{															\
		mServerClassName=typeid(TServer).name();				\
		pMServer=NULL;											\
	};															\
	bool AddLocalObject(TServer* pTServer)						\
	{															\
		pMServer=pTServer;return true;							\
	};															\
	bool IsRunning();											\
private:														\
	std::string mServerClassName;								\
	TServer* pMServer;											\
	void CallLocalFunction(const char* pFuncID, void* pParaList,int tParaListLenght,SOCKET tSocket)\
	{															\
		std::string FunTempID;


#define MY_RMI_SERVER_FUNCTION_ADD_P0(R,FunName)				\
	if(strcmp(JOINSTRING2(R,FunName),pFuncID)==0)					\
	{															\
		R m##R##FunName=pMServer->FunName();					\
		SendResponse(&m##R##FunName,sizeof(m##R##FunName),tSocket);		\
		return ;												\
	}

#define MY_RMI_SERVER_FUNCTION_ADD_P1(R,FunName,P1)				\
	if(strcmp(JOINSTRING3(R,FunName,P1),pFuncID)==0)					\
	{															\
		R m##R##FunName##P1=pMServer->FunName(ParaListAnalyser<R,P1>(pParaList).GetP1());\
		SendResponse(&m##R##FunName##P1,sizeof(m##R##FunName##P1),tSocket);			\
		return ;												\
	}

#define MY_RMI_SERVER_FUNCTION_ADD_P2(R,FunName,P1,P2)			\
	if(strcmp(JOINSTRING4(R,FunName,P1,P2),pFuncID)==0)					\
	{															\
		R m##R##FunName##P1##P2=pMServer->FunName(ParaListAnalyser<R,P1,P2>(pParaList).GetP1(),\
					ParaListAnalyser<R,P1,P2>(pParaList).GetP2());\
		SendResponse(&m##R##FunName##P1##P2,sizeof(m##R##FunName##P1##P2),tSocket);			\
		return ;												\
	}

#define MY_RMI_SERVER_FUNCTION_ADD_P3(R,FunName,P1,P2,P3)			\
	if(strcmp(JOINSTRING5(R,FunName,P1,P2,P3),pFuncID)==0)					\
	{															\
		R m##R##FunName##P1##P2##P3=pMServer->FunName(ParaListAnalyser<R,P1,P2,P3>(pParaList).GetP1(),\
					ParaListAnalyser<R,P1,P2,P3>(pParaList).GetP2(),\
					ParaListAnalyser<R,P1,P2,P3>(pParaList).GetP3());\
		SendResponse(&m##R##FunName##P1##P2##P3,sizeof(m##R##FunName##P1##P2##P3),tSocket);			\
		return ;												\
	}

#define MY_RMI_SERVER_FUNCTION_ADD_P4(R,FunName,P1,P2,P3,P4)			\
	if(strcmp(JOINSTRING6(R,FunName,P1,P2,P3,P4),pFuncID)==0)					\
	{															\
		R m##R##FunName##P1##P2##P3##P4=pMServer->FunName(ParaListAnalyser<R,P1,P2,P3,P4>(pParaList).GetP1(),\
					ParaListAnalyser<R,P1,P2,P3,P4>(pParaList).GetP2(),\
					ParaListAnalyser<R,P1,P2,P3,P4>(pParaList).GetP3(),\
					ParaListAnalyser<R,P1,P2,P3,P4>(pParaList).GetP4());\
		SendResponse(&m##R##FunName##P1##P2##P3##P4,sizeof(m##R##FunName##P1##P2##P3##P4),tSocket);			\
		return ;												\
	}
#define MY_RMI_SERVER_FUNCTION_ADD_P5(R,FunName,P1,P2,P3,P4,P5)			\
	if(strcmp(JOINSTRING7(R,FunName,P1,P2,P3,P4,P5),pFuncID)==0)					\
	{															\
		R m##R##FunName##P1##P2##P3##P4##P5=pMServer->FunName(ParaListAnalyser<R,P1,P2,P3,P4,P5>(pParaList).GetP1(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5>(pParaList).GetP2(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5>(pParaList).GetP3(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5>(pParaList).GetP4(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5>(pParaList).GetP5());\
		SendResponse(&m##R##FunName##P1##P2##P3##P4##P5,sizeof(m##R##FunName##P1##P2##P3##P4##P5),tSocket);			\
		return ;												\
	}
#define MY_RMI_SERVER_FUNCTION_ADD_P6(R,FunName,P1,P2,P3,P4,P5,P6)			\
	if(strcmp(JOINSTRING8(R,FunName,P1,P2,P3,P4,P5,P6),pFuncID)==0)					\
	{															\
		R m##R##FunName##P1##P2##P3##P4##P5##P6=pMServer->FunName(ParaListAnalyser<R,P1,P2,P3,P4,P5,P6>(pParaList).GetP1(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6>(pParaList).GetP2(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6>(pParaList).GetP3(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6>(pParaList).GetP4(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6>(pParaList).GetP5(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6>(pParaList).GetP6());\
		SendResponse(&m##R##FunName##P1##P2##P3##P4##P5##P6,sizeof(m##R##FunName##P1##P2##P3##P4##P5##P6),tSocket);			\
		return ;												\
	}
#define MY_RMI_SERVER_FUNCTION_ADD_P7(R,FunName,P1,P2,P3,P4,P5,P6,P7)			\
	if(strcmp(JOINSTRING9(R,FunName,P1,P2,P3,P4,P5,P6,P7),pFuncID)==0)					\
	{															\
		R m##R##FunName##P1##P2##P3##P4##P5##P6##P7=pMServer->FunName(ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7>(pParaList).GetP1(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7>(pParaList).GetP2(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7>(pParaList).GetP3(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7>(pParaList).GetP4(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7>(pParaList).GetP5(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7>(pParaList).GetP6(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7>(pParaList).GetP7());\
		SendResponse(&m##R##FunName##P1##P2##P3##P4##P5##P6##P7,sizeof(m##R##FunName##P1##P2##P3##P4##P5##P6##P7),tSocket);			\
		return ;												\
	}
#define MY_RMI_SERVER_FUNCTION_ADD_P8(R,FunName,P1,P2,P3,P4,P5,P6,P7,P8)			\
	if(strcmp(JOINSTRING10(R,FunName,P1,P2,P3,P4,P5,P6,P7,P2),pFuncID)==0)					\
	{															\
		R m##R##FunName##P1##P2##P3##P4##P5##P6##P7##P8=pMServer->FunName(ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP1(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP2(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP3(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP4(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP5(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP6(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP7(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP8());\
		SendResponse(&m##R##FunName##P1##P2##P3##P4##P5##P6##P7##P8,sizeof(m##R##FunName##P1##P2##P3##P4##P5##P6##P7##P8),tSocket);			\
		return ;												\
	}
#define MY_RMI_SERVER_FUNCTION_ADD_P9(R,FunName,P1,P2,P3,P4,P5,P6,P7,P8,P9)			\
	if(strcmp(JOINSTRING11(R,FunName,P1,P2,P3,P4,P5,P6,P7,P8,P9),pFuncID)==0)					\
	{															\
		R m##R##FunName##P1##P2##P3##P4##P5##P6##P7##P8##P9=pMServer->FunName(ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP1(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP2(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP3(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP4(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP5(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP6(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP7(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP8(),\
					ParaListAnalyser<R,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP9());\
		SendResponse(&m##R##FunName##P1##P2##P3##P4##P5##P6##P7##P8##P9,sizeof(m##R##FunName##P1##P2##P3##P4##P5##P6##P7##P8##P9),tSocket);			\
		return ;												\
	}

//以下是没有参数的的函数声明
#define MY_RMI_SERVER_FUNCTION_ADD_V_P0(FunName)				\
	if(strcmp(JOINSTRING2(void,FunName),pFuncID)==0)					\
	{															\
		pMServer->FunName();									\
		return ;												\
	}

#define MY_RMI_SERVER_FUNCTION_ADD_V_P1(FunName,P1)				\
	if(strcmp(JOINSTRING3(void,FunName,P1),pFuncID)==0)					\
	{															\
		pMServer->FunName(ParaListAnalyser<MYVOIDCLASS,P1>(pParaList).GetP1());\
		return ;												\
	}

#define MY_RMI_SERVER_FUNCTION_ADD_V_P2(FunName,P1,P2)			\
	if(strcmp(JOINSTRING4(void,FunName,P1,P2),pFuncID)==0)					\
	{															\
		pMServer->FunName(ParaListAnalyser<MYVOIDCLASS,P1,P2>(pParaList).GetP1(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2>(pParaList).GetP2());\
		return ;												\
	}

#define MY_RMI_SERVER_FUNCTION_ADD_V_P3(FunName,P1,P2,P3)			\
	if(strcmp(JOINSTRING5(void,FunName,P1,P2,P3),pFuncID)==0)					\
	{															\
		pMServer->FunName(ParaListAnalyser<MYVOIDCLASS,P1,P2,P3>(pParaList).GetP1(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3>(pParaList).GetP2(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3>(pParaList).GetP3());\
		return ;												\
	}

#define MY_RMI_SERVER_FUNCTION_ADD_V_P4(FunName,P1,P2,P3,P4)			\
	if(strcmp(JOINSTRING6(void,FunName,P1,P2,P3,P4),pFuncID)==0)					\
	{															\
		pMServer->FunName(ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4>(pParaList).GetP1(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4>(pParaList).GetP2(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4>(pParaList).GetP3(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4>(pParaList).GetP4());\
		return ;												\
	}
#define MY_RMI_SERVER_FUNCTION_ADD_V_P5(FunName,P1,P2,P3,P4,P5)			\
	if(strcmp(JOINSTRING7(void,FunName,P1,P2,P3,P4,P5),pFuncID)==0)					\
	{															\
		pMServer->FunName(ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5>(pParaList).GetP1(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5>(pParaList).GetP2(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5>(pParaList).GetP3(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5>(pParaList).GetP4(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5>(pParaList).GetP5());\
		return ;												\
	}
#define MY_RMI_SERVER_FUNCTION_ADD_V_P6(FunName,P1,P2,P3,P4,P5,P6)			\
	if(strcmp(JOINSTRING8(void,FunName,P1,P2,P3,P4,P5,P6),pFuncID)==0)					\
	{															\
		pMServer->FunName(ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6>(pParaList).GetP1(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6>(pParaList).GetP2(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6>(pParaList).GetP3(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6>(pParaList).GetP4(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6>(pParaList).GetP5(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6>(pParaList).GetP6());\
		return ;												\
	}
#define MY_RMI_SERVER_FUNCTION_ADD_V_P7(FunName,P1,P2,P3,P4,P5,P6,P7)			\
	if(strcmp(JOINSTRING9(void,FunName,P1,P2,P3,P4,P5,P6,P7),pFuncID)==0)					\
	{															\
		pMServer->FunName(ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7>(pParaList).GetP1(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7>(pParaList).GetP2(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7>(pParaList).GetP3(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7>(pParaList).GetP4(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7>(pParaList).GetP5(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7>(pParaList).GetP6(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7>(pParaList).GetP7());\
		return ;												\
	}
#define MY_RMI_SERVER_FUNCTION_ADD_V_P8(FunName,P1,P2,P3,P4,P5,P6,P7,P8)			\
	if(strcmp(JOINSTRING10(void,FunName,P1,P2,P3,P4,P5,P6,P7,P2),pFuncID)==0)					\
	{															\
		pMServer->FunName(ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP1(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP2(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP3(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP4(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP5(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP6(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP7(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8>(pParaList).GetP8());\
		return ;												\
	}
#define MY_RMI_SERVER_FUNCTION_ADD_V_P9(FunName,P1,P2,P3,P4,P5,P6,P7,P8,P9)			\
	if(strcmp(JOINSTRING11(void,FunName,P1,P2,P3,P4,P5,P6,P7,P8,P9),pFuncID)==0)					\
	{															\
		pMServer->FunName(ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP1(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP2(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP3(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP4(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP5(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP6(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP7(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP8(),\
					ParaListAnalyser<MYVOIDCLASS,P1,P2,P3,P4,P5,P6,P7,P8,P9>(pParaList).GetP9());\
		return ;												\
	}



#define MY_RMI_SERVER_CLASS_END()								\
	char* ptemp="错误:所请求的方法不存在\r\n";					\
	SendResponse((void*)ptemp,strlen(ptemp),tSocket);					\
	}															\
};




#endif