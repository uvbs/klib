/*******************************************************************************
*
* FileName : slist.h
* Comment  : single linked list
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-21 20:24
*
*******************************************************************************/

#ifndef _SLIST_H_INCLUDED
#define _SLIST_H_INCLUDED

#include <cassert>

template<typename T>
struct Slist_node {
	T element;
	Slist_node* next;

	Slist_node() : element(), next(0) {}
	explicit Slist_node(const T& elem) : element(elem), next(0) {}
	~Slist_node() { next=0; }
};

template<typename T>
class Slist {

	typedef Slist_node<T>* Ptrn;

public:
	Slist() : hb_head(new Slist_node<T>), hb_size(0) {}
	~Slist();

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
	Slist_node<T>* hb_head;
	int hb_size;
};

template<typename T>
Slist<T>::~Slist()
{
	clear();
	delete hb_head;
}

template<typename T>
void Slist<T>::clear()
{
	if (hb_size != 0)
		erase(1,hb_size);
}

template<typename T>
void Slist<T>::erase(int beg, int end)
{
	if (beg>hb_size || end > hb_size || beg<=0 || end<=0 || beg>end)
		std::cerr<<"error : position out of range!\n";
	else {
		Ptrn ptr=hb_head;
		Ptrn prev=0;
		int i=0;
		while (i != beg) {
			prev=ptr;
			ptr=ptr->next;
			++i;
		}
		while (i <= end) {
			Ptrn ptm=ptr;
			ptr=ptr->next;
			delete ptm;
			--hb_size;
			++i;
		}
		prev->next=ptr;
	}
}

template<typename T>
T Slist<T>::erase(int pos)
{
	assert(pos<=hb_size && pos!=0);
	Ptrn ptr=hb_head;
	Ptrn prev=0;
	int i=0;
	while (i != pos) {
		prev=ptr;
		ptr=ptr->next;
		++i;
	}
	T t=ptr->element;
	prev->next=ptr->next;
	delete ptr;
	--hb_size;
	return t;
}

template<typename T>
void Slist<T>::insert_after(const T& elem, int pos)
{
	Ptrn ptr=hb_head;
	int i=0;
	while ( i!= pos) {
		ptr=ptr->next;
		++i;
	}

	Slist_node<T>* pSln=new Slist_node<T>;
	pSln->element=elem;
	pSln->next=ptr->next;
	ptr->next=pSln;
	++hb_size;
}

template<typename T>
void Slist<T>::push_front(const T& elem)
{
	insert_after(elem,0);
}

template<typename T>
void Slist<T>::push_back(const T& elem)
{
	insert_after(elem,hb_size);
}

template<typename T>
T Slist<T>::pop_front()
{
	return erase(1);
}

template<typename T>
T Slist<T>::pop_back()
{
	return erase(hb_size);
}

template<typename T>
inline T Slist<T>::value(int pos) const
{
	Ptrn ptr=hb_head;
	int i=0;
	while (i != pos) {
		ptr=ptr->next;
		++i;
	}
	return ptr->element;
}

template<typename T>
void Slist<T>::reverse()
{
	Ptrn pbeg=hb_head->next;
	hb_head->next=0;
	hb_size=0;
	Ptrn ptr=pbeg;
	while (ptr != 0) {
		push_front(ptr->element);
		Ptrn ptm=ptr;
		ptr=ptr->next;
		delete ptm;
	}
}

#endif