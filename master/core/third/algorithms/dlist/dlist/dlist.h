/*******************************************************************************
*
* FileName : dlist.h
* Comment  : doubly linked list
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-21 20:17
*
*******************************************************************************/

#ifndef _DLIST_H_INCLUDED
#define _DLIST_H_INCLUDED

#include <cassert>
#include <iostream>
template<typename T>
struct Dlist_node {
	T element;
	Dlist_node* next;
	Dlist_node* prev;

	Dlist_node() : element(), next(0), prev(0) {}
	explicit Dlist_node(const T& elem) : element(elem), next(0), prev(0) {}
	~Dlist_node() { next=prev=0; }
};

template<typename T>
class Dlist {

	typedef Dlist_node<T>* Ptrn;

public:
	Dlist();
	~Dlist();

	bool empty() const { return hb_size ? false : true; }
	int size() const { return hb_size; }
	void reverse();
//	void sort() const;
	T value(int pos) const;

	void push_front(const T& elem);
	T pop_front();
	void push_back(const T& elem);
	T pop_back();

	void insert_after(const T& elem, int pos);
	T erase(int pos);
	void erase(int beg, int end);
	void clear();

private:
	Dlist_node<T>* hb_head;
	Dlist_node<T>* hb_tail;
	int hb_size;

	Dlist_node<T>* find(int pos) const;
	T erase(Dlist_node<T>* pSln);
};

template<typename T>
Dlist<T>::Dlist() : hb_head(new Dlist_node<T>),
					hb_tail(new Dlist_node<T>), hb_size(0)
{
	hb_head->next=hb_tail;
	hb_head->prev=0;
	hb_tail->prev=hb_head;
	hb_tail->next=0;
}

template<typename T>
Dlist<T>::~Dlist()
{
	clear();
	delete hb_head;
	delete hb_tail;
}

template<typename T>
void Dlist<T>::clear()
{
	if (hb_size != 0)
		erase(1,hb_size);
}

template<typename T>
Dlist_node<T>* Dlist<T>::find(int pos) const
{
	if (pos<0 || pos>hb_size) {
		std::cerr<<"error, position out of range!\n";
		return 0;
	}

	Ptrn ptr=hb_head;
	int i=0;
	while (i != pos) {
		ptr=ptr->next;
		++i;
	}

	return ptr;
}

template<typename T>
void Dlist<T>::insert_after(const T& elem, int pos)
{
	Ptrn pbef=find(pos);
	if (0 == pbef) {
		return;
	}
	Ptrn paft=pbef->next;
	Ptrn pSln=new Dlist_node<T>;
	pSln->element=elem;
	pbef->next=pSln;
	pSln->prev=pbef;
	paft->prev=pSln;
	pSln->next=paft;
	++hb_size;
}

template<typename T>
void Dlist<T>::push_front(const T& elem)
{
	insert_after(elem,0);
}

template<typename T>
void Dlist<T>::push_back(const T& elem)
{
	insert_after(elem,hb_size);
}

template<typename T>
T Dlist<T>::erase(Dlist_node<T>* pSln)
{
	pSln->prev->next=pSln->next;
	pSln->next->prev=pSln->prev;
	T t=pSln->element;
	delete pSln;
	--hb_size;
	return t;
}

template<typename T>
T Dlist<T>::erase(int pos)
{
	Ptrn pSln=find(pos);
	assert(pSln != 0);
	return erase(pSln);
}

template<typename T>
T Dlist<T>::pop_front()
{
	return erase(1);
}

template<typename T>
T Dlist<T>::pop_back()
{
	return erase(hb_size);
}

template<typename T>
void Dlist<T>::erase(int beg, int end)
{
	Ptrn pbeg=find(beg);
	Ptrn pend=find(end);
	if (0==pbeg || 0==pend) {
		return;
	}
	pbeg->prev->next=pend->next;
	pend->next->prev=pbeg->prev;

	Ptrn pstop=pend->next;
	while (pbeg != pstop) {
		Ptrn ptm=pbeg;
		pbeg=pbeg->next;
		delete ptm;
		--hb_size;
	}
}

template<typename T>
T Dlist<T>::value(int pos) const
{
	Ptrn pSln=find(pos);
	assert(0 != pSln);
	return pSln->element;
}

template<typename T>
void Dlist<T>::reverse()
{
	Ptrn ptr=hb_head;
	while (ptr != hb_tail) {
		ptr->prev=ptr->next;
		ptr=ptr->next;
	}

	ptr=hb_head;
	while (ptr != hb_tail) {
		Ptrn pbef=ptr;
		ptr=ptr->prev;
		ptr->next=pbef;
	}

	Ptrn head=hb_head;
	hb_head=hb_tail;
	hb_tail=head;
}

#endif