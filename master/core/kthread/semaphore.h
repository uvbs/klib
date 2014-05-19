#pragma once
#include <Windows.h>
#include <string>
using namespace std;


//该类封装了Win32信号量同步机制，支持线程、进程间同步
//信号量同步机制，支持一个线程发送信号，一个线程等待信号,每发送一个信号
//内核对象的信号计数加1，每等待到一个信号内核对象信号计数减1，当内核对象
//信号计数为0时，等待操作将被阻塞，直到信号计数大于0或等待超时

#include "../istddef.h"

namespace klib {
namespace kthread{


/**
 * @brief 信号量实现类
 */
class semaphore
{
public:
	semaphore(int usercount, int maxcount, tstring name);
	~semaphore(void);

    /**
     * @brief 信号计数加1,释放信号量
     */
	void release(int n = 1);

    /**
     * @brief 等待信号，信号计数减1
     */
	void wait(DWORD dwMilliseconds = INFINITE);

    /**
     * @brief 
     */
	void close();

private:
	HANDLE  m_hSemaphore;               ///< 信号量句柄
};


}}
