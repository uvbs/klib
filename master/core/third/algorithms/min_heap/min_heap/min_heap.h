/*******************************************************************************
*
* FileName : min_heap.h
* Comment  : minimal heap
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-25 10:07
*
*******************************************************************************/

#ifndef _MIN_HEAP_H_INCLUDED
#define _MIN_HEAP_H_INCLUDED

#include "../../heap/heap/heap.h"
#include <iostream>
template<typename T>
class Min_heap : public Heap<T> {
public:
	explicit Min_heap(int max_size, const T lower_limit);
	~Min_heap();

	virtual void insert(const T& elem);
	T erase_min();
	T find_min();

	virtual void decrease_key(int p, T val);
	virtual void increase_key(int p, T val);
};

template<typename T>
Min_heap<T>::Min_heap(int max_size, const T lower_limit)
			: Heap(max_size, lower_limit)
{
}

template<typename T>
Min_heap<T>::~Min_heap()
{
}

template<typename T>
void Min_heap<T>::insert(const T& elem)
{
	if (full()) {
		std::cerr<<"error, the heap is full!\n";
		return;
	}
	int i=++hb_size;
	for (; hb_element[i>>1] > elem; i=i>>1)
		hb_element[i] = hb_element[i>>1];
	hb_element[i] = elem;
}

template<typename T>
T Min_heap<T>::erase_min()
{
	if (empty()) {
		std::cerr<<"error, the heap is empty!\n";
		return hb_element[0];
	}

	T min_element = hb_element[1];	//return value
	T last_element = hb_element[hb_size--];	//pick up the last element

	int i, child;
	for (i=1; (i<<1) <= hb_size; i=child) {
		child = i<<1;
		if (child!=hb_size && hb_element[child] > hb_element[child+1])
			child++;	//choose the little child
		if (last_element <= hb_element[child])
			break;
		hb_element[i] = hb_element[child];
	}
	hb_element[i] = last_element;
	return min_element;
}

template<typename T>
T Min_heap<T>::find_min()
{
	if (empty()) {
		std::cerr<<" the heap is empty!\n";
		return hb_element[0];
	}
	return hb_element[1];
}

template<typename T>
void Min_heap<T>::decrease_key(int p, T val)
{
	if (p > hb_size) {
		std::cerr<<"error, the element not exist!\n";
		return;
	}
	int elem = hb_element[p] - val;
	if (elem <= hb_element[0]) {
		std::cerr<<"error, the key decrease too much!\n";
		return;
	}
	hb_element[p] = elem;
	int i = p;
	for (; i!=0; i=i>>1) {
		if (elem < hb_element[i>>1])
			hb_element[i] = hb_element[i>>1];
		else
			break;
	}
	hb_element[i] = elem;
}

template<typename T>
void Min_heap<T>::increase_key(int p, T val)
{
	if (p > hb_size) {
		std::cerr<<"error, the element not exist!\n";
		return;
	}
	hb_element[p] += val;
	int elem = hb_element[p];
	int i = p;
	int child;
	for (; i!=hb_size; i=child) {
		child = i<<1;
		if (child!=hb_size && hb_element[child] > hb_element[child+1])
			child++;	//choose the little child
		if (elem > hb_element[child])
			hb_element[i] = hb_element[child];
		else
			break;
	}
	hb_element[i] = elem;
}

#endif