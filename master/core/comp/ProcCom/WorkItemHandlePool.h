#pragma once
#include<Windows.h>
#include <string>
#include <functional>
using namespace std;

class CWorkItemHandlePool
{
public:
	CWorkItemHandlePool(void);
    ~CWorkItemHandlePool(void);

    /**
     * @brief 回调函数对象
     */
    typedef tr1::function<void(string)> WorkerType;


    /**
     * @brief  提交工作队列
     */
	void SubmitWork(WorkerType fun, string str);

private:
    /**
     * @brief 回调函数
     */
	static DWORD WINAPI ThreadPoolCallback(LPVOID pvContext);

private:
	struct WorkItem
	{
	public:
		WorkItem(WorkerType Cmd,string str)
		{
			cmd = Cmd;
			inparam = str;
		}
		~WorkItem()
		{}

		WorkerType cmd;
		string inparam;
	};


};
