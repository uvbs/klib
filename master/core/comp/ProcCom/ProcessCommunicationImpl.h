#pragma once

//进程间通信实现类
//该类实现两个进程间相互通信

#include <map>
#include <deque>
#include "WRCommunicate.h"
#include "WorkItemHandlePool.h"
using namespace std;

#include "../../kthread/auto_lock.h"
using namespace klib::kthread;

class CProcessRecvData
{
public:
	CProcessRecvData();
	virtual ~CProcessRecvData();
	virtual void handle_read(string str);

};


template<typename T = CProcessRecvData>
class CProcessCommunicationImpl
{
public:
	//构造函数
	CProcessCommunicationImpl()
	{
		m_pHandler = new T();
	}

	~CProcessCommunicationImpl(void){}

	//读模式连接到一块内存区域
	void RConnect(tstring name,int buffersize)
	{
		InitWRCommunicate(name, buffersize, true);
	}

	//写模式连接到一块内存区域
	void WConnect(tstring name, int buffersize)
	{
		InitWRCommunicate(name, buffersize);
	}

	//向一块已经连接的内存写数据
	void write(tstring wname,string str)
	{
		auto_lock lock(m_objAutoCS);

		auto itr = m_pCommunicateSet.find(wname);
		if(itr != m_pCommunicateSet.end())
        {
			itr->second->write(str);
		}
	}

	//关闭指定的连接
	void close(tstring name)
	{
		auto_lock lock(m_objAutoCS);

		auto itr = m_pCommunicateSet.find(name);
		if(itr != m_pCommunicateSet.end())
        {
			itr->second->close();
			delete (itr->second);
			m_pCommunicateSet.erase(itr);
		}
	}

	//关闭所有连接
	void close_all()
	{
        auto_lock lock(m_objAutoCS);

		auto itr = m_pCommunicateSet.begin();
		while(itr != m_pCommunicateSet.end())
        {
			(itr->second)->close();
			delete (itr->second);
		}
		m_pCommunicateSet.clear();
	}

private:
	static DWORD WINAPI ProcessReadThreadPro(LPVOID lpParameter)
	{
		CProcessCommunicationImpl* p = (CProcessCommunicationImpl*)lpParameter;
		CWRCommunicate* pCommunicate = p->getnewCommunicate();
		if(pCommunicate == NULL)
        {
			return 0;
        }

		while(!pCommunicate->isEnd())
		{
			string str;
			pCommunicate->read(str);
			p->SubmitWork(str);
		}
		return 0;
	}

private:
	CWRCommunicate* getnewCommunicate()
	{
		if(m_wait_deal.size() > 0)
        {
			auto_lock lock(m_objAutoCS);

			tstring name = m_wait_deal.front();
			m_wait_deal.pop_front();
			auto itr = m_pCommunicateSet.find(name);
			if(itr != m_pCommunicateSet.end())
            {
				CWRCommunicate* pwr = itr->second;
				
				return pwr;
			}
		}

		return NULL;
	}

	void handle(string str)
	{
		m_pHandler->handle_read(str);
	}

	void SubmitWork(string str)
	{
		m_workhandle.SubmitWork(
			tr1::bind(&CProcessCommunicationImpl::handle,
			this,tr1::placeholders::_1),
		    str
        );
	}

	//初始化一个进程通信模块
	void InitWRCommunicate(tstring name, int buffersize, bool bRead = false)
	{
		auto_lock lock(m_objAutoCS);

		m_wait_deal.push_back(name);
		CWRCommunicate* p = new CWRCommunicate(name, buffersize);
		m_pCommunicateSet.insert(std::make_pair(name, p));

		if(bRead)
        {
			CreateThread(NULL, 
                0, 
                &CProcessCommunicationImpl::ProcessReadThreadPro,
			    (LPVOID)this,
                0,
                NULL
            );
		}
	}

private:
	map<tstring,CWRCommunicate*>        m_pCommunicateSet;	//进程通信集合
	share_memory                        m_ShareMemory;
	CWorkItemHandlePool                 m_workhandle;
	T*                                  m_pHandler;
	deque<tstring>                       m_wait_deal;		//等待处理的读对象
    auto_cs                             m_objAutoCS;
};

