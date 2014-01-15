// ProcessCommunicate.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "../../core/comp/ProcCom/ProcessCommunicationImpl.h"
#include <iostream>
using namespace std;

#include "../../core/klib_link.h"

int _tmain(int argc, _TCHAR* argv[])
{
    CProcessCommunicationImpl<> processcommunicate;
    tstring rname;
    cout<<"读取缓冲区名称：";
    wcin>>rname;
    processcommunicate.RConnect(rname,1000);

    cout<<"写入缓冲区名称：";
    tstring wname;
    wcin>>wname;
    processcommunicate.WConnect(wname,1000);

    while(1)
    {
        string str;
        cout<<"输入要发送的内容:";
        cin>>str;
        if(str == "xh")
        {
            char buf[10];
            for(int i=1;i<900000000;++i)
            {
                memset(buf,0,10);
                _itoa_s(i,buf,10);
                string str = buf;
                str+=":这是一条测试数据不用回复，赵海彬。";
                processcommunicate.write(wname,str);
                //Sleep(10);
            }
        }
        if(str != "exitw" && str != "wxitr" && str != "exitall")
            processcommunicate.write(wname,str);
        if(str == "exitw")
            processcommunicate.close(wname);
        if(str == "exitr" )
            processcommunicate.close(rname);
        if(str == "exitall")
            processcommunicate.close_all();
    }


	return 0;
}

