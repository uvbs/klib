/*******************************************************************************
*
* FileName : queue.h
* Comment  : queue implemented in terms of single linked list
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-21 :
*
*******************************************************************************/

#ifndef _QUEUE_H_INCLUDED
#define _QUEUE_H_INCLUDED

#include "../../slist/slist/slist.h"

template<typename T>
class Queue {
public:
	Queue() : hb_slist() {}
	~Queue() {}

	bool empty() const { return hb_slist.empty(); }
	void clear() { hb_slist.clear(); }
	int size() const { return hb_slist.size(); }

	void en_queue(const T& elem) { hb_slist.push_back(elem); }
	T de_queue() { return hb_slist.pop_front(); }
	T front() const { return hb_slist.value(1); }
	T back() const { return hb_slist.value(hb_slist.size()); }

private:
	Slist<T> hb_slist;
};

#endif