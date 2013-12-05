//#include "stdafx.h"
#include "WorkItemHandlePool.h"

CWorkItemHandlePool::CWorkItemHandlePool(void)
{

}

CWorkItemHandlePool::~CWorkItemHandlePool(void)
{
}

void CWorkItemHandlePool::SubmitWork(WorkerType fun,string str)
{
	WorkItem* pWorkItem = new WorkItem(fun, str);
	QueueUserWorkItem(
		CWorkItemHandlePool::ThreadPoolCallback,
		(LPVOID)pWorkItem,
		WT_EXECUTELONGFUNCTION);
}

DWORD WINAPI CWorkItemHandlePool::ThreadPoolCallback( LPVOID pvContext )
{
    WorkItem* pWorkItem = (WorkItem*)pvContext;
    (pWorkItem->cmd)(pWorkItem->inparam);
    delete pWorkItem;
    return 0;
}

