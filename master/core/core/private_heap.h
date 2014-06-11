/*
 * Copyright Bruce Liang (ldcsaa@gmail.com)
 *
 * Version	: 2.2.1
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Porject	: https://code.google.com/p/ldcsaa
 * Bolg		: http://www.cnblogs.com/ldcsaa
 * WeiBo	: http://weibo.com/u/1402935851
 * QQ Group	: 75375912
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
/******************************************************************************
Module:  PrivateHeap.h
Notices: Copyright (c) 2006 Bruce Liang
Purpose: 管理进程私有堆.
Desc:
		 1. private_heap:		自动创建和销毁进程私有堆
								每一个该类的对象都代表一个私有堆, 所以
								该类对象的特点是: 一般声明周期都比较长
								通常作为全局对象, 其他类的静态成员对象
								或者一些长生命周期类对象的成员对象
		 2. private_heap_buffer: 在私有堆中自动分配和释放指定大小的内存
								一般用于在函数体内分配和释放局部作用域的堆内存
								从而避免对 private_heap::Alloc() 和 
								private_heap::Free() 的调用

Examples:
			private_heap g_hpPrivate;

			int _tmain(int argc, _TCHAR* argv[])
			{
				CPrivateHeapStrBuffer buff(g_hpPrivate, 32);
				lstrcpy(buff, _T("失败乃成功之母"));
				SIZE_T size = buff.Size();
				buff.ReAlloc(40);
				size = buff.Size();
				std::cout << (TCHAR*)buff << '\n';
				// OR
				// _ASSERT(g_hpPrivate.IsValid());
				// TCHAR* pch	= (TCHAR*)g_hpPrivate.Alloc(32 * sizeof(TCHAR));
				// lstrcpy(pch, _T("失败乃成功之母"));
				// SIZE_T size = g_hpPrivate.Size(pch);
				// g_hpPrivate.ReAlloc(pch, 40 * sizeof(TCHAR));
				// size = g_hpPrivate.Size(pch);
				// std::cout << pch << '\n';
				// g_hpPrivate.Free(pch);
				// 
				return 0;
			}

******************************************************************************/

#ifndef _klib_private_heap_h
#define _klib_private_heap_h


namespace klib {
namespace mem {


/**
 * @brief 私堆类
 */

class private_heap
{
public:

	/*
	enum EnCreateOptions
	{
		CO_DEFAULT				= 0,
		CO_NO_SERIALIZE			= HEAP_NO_SERIALIZE,
		CO_GENERATE_EXCEPTIONS	= HEAP_GENERATE_EXCEPTIONS,
		CO_NOSERIALIZE_GENERATEEXCEPTIONS
								= HEAP_NO_SERIALIZE	|
								  HEAP_GENERATE_EXCEPTIONS
	};

	enum EnAllocOptions
	{
		AO_DEFAULT				= 0,
		AO_ZERO_MEMORY			= HEAP_ZERO_MEMORY,
		AO_NO_SERIALIZE			= HEAP_NO_SERIALIZE,
		AO_GENERATE_EXCEPTIONS	= HEAP_GENERATE_EXCEPTIONS,
		AO_ZEROMEMORY_NOSERIALIZE
								= HEAP_ZERO_MEMORY	|
								  HEAP_NO_SERIALIZE,
		AO_ZEROMEMORY_GENERATEEXCEPTIONS
								= HEAP_ZERO_MEMORY	|
								  HEAP_GENERATE_EXCEPTIONS,
		AO_NOSERIALIZE_GENERATESEXCEPTIONS
								= HEAP_NO_SERIALIZE	|
								  HEAP_GENERATE_EXCEPTIONS,
		AO_ZEROMEMORY_NOSERIALIZE_GENERATESEXCEPTIONS
								= HEAP_ZERO_MEMORY	|
								  HEAP_NO_SERIALIZE	|
								  HEAP_GENERATE_EXCEPTIONS
	};

	enum EnReAllocOptions
	{
		RAO_DEFAULT					= 0,
		RAO_ZERO_MEMORY				= HEAP_ZERO_MEMORY,
		RAO_NO_SERIALIZE			= HEAP_NO_SERIALIZE,
		RAO_GENERATE_EXCEPTIONS		= HEAP_GENERATE_EXCEPTIONS,
		RAO_REALLOC_IN_PLACE_ONLY	= HEAP_REALLOC_IN_PLACE_ONLY,
		RAO_ZEROMEMORY_NOSERIALIZE
									= HEAP_ZERO_MEMORY			|
									  HEAP_NO_SERIALIZE,
		RAO_ZEROMEMORY_GENERATEEXCEPTIONS
									= HEAP_ZERO_MEMORY			|
									  HEAP_GENERATE_EXCEPTIONS,
		RAO_ZEROMEMORY_REALLOCINPLACEONLY
									= HEAP_ZERO_MEMORY			|
									  HEAP_REALLOC_IN_PLACE_ONLY,
		RAO_NOSERIALIZE_GENERATESEXCEPTIONS
									= HEAP_NO_SERIALIZE			|
									  HEAP_GENERATE_EXCEPTIONS,
		RAO_NOSERIALIZE_REALLOCINPLACEONLY
									= HEAP_NO_SERIALIZE			|
									  HEAP_REALLOC_IN_PLACE_ONLY,
		RAO_GENERATESEXCEPTIONS_REALLOCINPLACEONLY
									= HEAP_GENERATE_EXCEPTIONS	|
									  HEAP_REALLOC_IN_PLACE_ONLY,
		RAO_ZEROMEMORY_NOSERIALIZE_GENERATESEXCEPTIONS
									= HEAP_ZERO_MEMORY			|
									  HEAP_NO_SERIALIZE			|
									  HEAP_GENERATE_EXCEPTIONS,
		RAO_ZEROMEMORY_NOSERIALIZE_REALLOCINPLACEONLY
									= HEAP_ZERO_MEMORY			|
									  HEAP_NO_SERIALIZE			|
									  HEAP_REALLOC_IN_PLACE_ONLY,
		RAO_ZEROMEMORY_GENERATESEXCEPTIONS_REALLOCINPLACEONLY
									= HEAP_ZERO_MEMORY			|
									  HEAP_GENERATE_EXCEPTIONS	|
									  HEAP_REALLOC_IN_PLACE_ONLY,
		RAO_NOSERIALIZE_GENERATESEXCEPTIONS_REALLOCINPLACEONLY
									= HEAP_NO_SERIALIZE			|
									  HEAP_GENERATE_EXCEPTIONS	|
									  HEAP_REALLOC_IN_PLACE_ONLY,
		RAO_ZEROMEMORY_NOSERIALIZE_GENERATESEXCEPTIONS_REALLOCINPLACEONLY
									= HEAP_ZERO_MEMORY			|
									  HEAP_NO_SERIALIZE			|
									  HEAP_GENERATE_EXCEPTIONS	|
									  HEAP_REALLOC_IN_PLACE_ONLY
	};

	enum EnSizeOptions
	{
		SO_DEFAULT		= 0,
		SO_NO_SERIALIZE	= HEAP_NO_SERIALIZE
	};
	
	enum EnFreeOptions
	{
		FO_DEFAULT		= 0,
		FO_NO_SERIALIZE	= HEAP_NO_SERIALIZE
	};

	enum EnCompactOptions
	{
		CPO_DEFAULT		= 0,
		CPO_NO_SERIALIZE	= HEAP_NO_SERIALIZE
	};
	*/

public:
	PVOID alloc(SIZE_T dwSize, DWORD dwFlags = 0)
		{return ::HeapAlloc(m_hHeap, dwFlags, dwSize);}

	PVOID realloc(PVOID pvMemory, SIZE_T dwSize, DWORD dwFlags = 0)
		{return ::HeapReAlloc(m_hHeap, dwFlags, pvMemory, dwSize);}

	SIZE_T size(PVOID pvMemory, DWORD dwFlags = 0)
		{return ::HeapSize(m_hHeap, dwFlags, pvMemory);}

	BOOL free_(PVOID pvMemory, DWORD dwFlags = 0)
		{return ::HeapFree(m_hHeap, dwFlags, pvMemory);}

	SIZE_T compact(DWORD dwFlags = 0)
		{return ::HeapCompact(m_hHeap, dwFlags);}

	BOOL is_valid() {return m_hHeap != nullptr;}

	BOOL reset()
	{
		if(is_valid()) ::HeapDestroy(m_hHeap);
		m_hHeap = ::HeapCreate(m_dwOptions, m_dwInitSize, m_dwMaxSize);

		return is_valid();
	}

public:
	private_heap(DWORD dwOptions = 0, SIZE_T dwInitSize = 0, SIZE_T dwMaxSize = 0)
	: m_dwOptions(dwOptions), m_dwInitSize(dwInitSize), m_dwMaxSize(dwMaxSize)
		{m_hHeap = ::HeapCreate(m_dwOptions, m_dwInitSize, m_dwMaxSize);}

	~private_heap	()	{if(is_valid()) ::HeapDestroy(m_hHeap);}

	operator HANDLE	()	{return m_hHeap;}

private:
	private_heap(const private_heap&);
	private_heap operator = (const private_heap&);

private:
	HANDLE	m_hHeap;
	DWORD	m_dwOptions;
	SIZE_T	m_dwInitSize;
	SIZE_T	m_dwMaxSize;
};

template<class T>
class private_heap_buffer 
{
public:
	private_heap_buffer(private_heap& hpPrivate,
						SIZE_T dwSize		= 0,
						DWORD dwAllocFlags	= 0,
						DWORD dwFreeFlags	= 0)
	: m_hpPrivate(hpPrivate)
	, m_dwAllocFlags(dwAllocFlags)
	, m_dwFreeFlags(dwFreeFlags)
	, m_pvMemory(nullptr)
	{
		_ASSERT(m_hpPrivate.is_valid());
		alloc(dwSize);
	}

	~private_heap_buffer() {free_();}

public:

	T* alloc(SIZE_T dwSize)
	{
		if(is_valid())
			free_();

		return m_pvMemory = (T*)m_hpPrivate.alloc(dwSize * sizeof(T), m_dwAllocFlags);
	}

	T* realloc(SIZE_T dwSize, DWORD dwFlags = 0)
		{return m_pvMemory = (T*)m_hpPrivate.ReAlloc(m_pvMemory, dwSize * sizeof(T), dwFlags);}

	SIZE_T size(DWORD dwFlags = 0)
		{return m_hpPrivate.size(m_pvMemory, dwFlags);}

	BOOL free_()
	{
		BOOL isOK = TRUE;

		if(is_valid())
		{
			isOK = m_hpPrivate.free_(m_pvMemory, m_dwFreeFlags);
			m_pvMemory = nullptr;
		}

		return isOK;
	}

	BOOL is_valid()			{return m_pvMemory != nullptr;}
	operator T* ()	const	{return m_pvMemory;}

private:
	private_heap_buffer(const private_heap_buffer&);
	private_heap_buffer operator = (const private_heap_buffer&);

private:
	private_heap&	m_hpPrivate;
	T*				m_pvMemory;
	DWORD			m_dwAllocFlags;
	DWORD			m_dwFreeFlags;
};

typedef private_heap_buffer<BYTE>	CPrivateHeapByteBuffer;
typedef private_heap_buffer<TCHAR>	CPrivateHeapStrBuffer;



}}



#endif
