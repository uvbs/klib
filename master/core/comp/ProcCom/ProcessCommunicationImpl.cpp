#include "ProcessCommunicationImpl.h"
#include <iostream>

CProcessRecvData::CProcessRecvData()
{

}

CProcessRecvData::~CProcessRecvData()
{
    
}

void CProcessRecvData::handle_read( string str )
{
	static int count = 0;
	++count;
	cout<<"第"<<count<<"次收到数据:"<<str<<endl;
}
