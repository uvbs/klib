/*******************************************************************************
*
* FileName : stack.h
* Comment  : stack implemented in terms of single linked list
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-21 21:21
*
*******************************************************************************/

#ifndef _STACK_H_INCLUDED
#define _STACK_H_INCLUDED

#include "../../slist/slist/slist.h"

template<typename T>
class Stack {
public:
	Stack() : hb_slist() {}
	~Stack() {}

	bool empty() const { return hb_slist.empty(); }
	void clear() { hb_slist.clear(); }
	int size() const { return hb_slist.size(); }

	T top() const { return hb_slist.value(hb_slist.size()); }
	void push(const T& elem) { hb_slist.push_back(elem); }
	T pop() { return hb_slist.pop_back(); }

private:
	Slist<T> hb_slist;
};

#endif