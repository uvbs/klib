/*******************************************************************************
*
* FileName : leftist_heap.h
* Comment  : left tist heap
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-26 21:08
*
*******************************************************************************/

#ifndef _LEFTIST_HEAP_H_INCLUDED
#define _LEFTIST_HEAP_H_INCLUDED

#include "../../binary_tree/binary_tree/binary_tree.h"

//------------for use the binary tree as base class------------begin
template<typename T>
struct Lheap_node {
	T key;
	int npl;	//null path length

	//support implicit conversion
	Lheap_node(T k = T()) : key(k), npl(0) {}
};

template<typename T>
std::ostream& operator<<(std::ostream& output, const Lheap_node<T>& rhs)
{ 
	return output<<rhs.key; 
}

template<typename T>
bool operator<(const Lheap_node<T>& lhs, const Lheap_node<T>& rhs)
{
	return lhs.key < rhs.key;
}

template<typename T>
bool operator<=(const Lheap_node<T>& lhs, const Lheap_node<T>& rhs)
{
	return lhs.key <= rhs.key;
}

template<typename T>
bool operator>(const Lheap_node<T>& lhs, const Lheap_node<T>& rhs)
{
	return lhs.key > rhs.key;
}

template<typename T>
bool operator>=(const Lheap_node<T>& lhs, const Lheap_node<T>& rhs)
{
	return lhs.key >= rhs.key;
}

template<typename T>
bool operator==(const Lheap_node<T>& lhs, const Lheap_node<T>& rhs)
{
	return lhs.key == rhs.key;
}
//-------------for use the binary tree as base class------------------end


template<typename T>
class Left_heap : public Binary_tree<Lheap_node<T> > {
public:
	//two friend functions, can't be global function, 
	//because of the template argument deduction is not omnipotent
	friend PBtn merge_heap(PBtn heap_l, PBtn heap_r)
	{
		if (0 == heap_l)
			return heap_r;
		if (0 == heap_r)
			return heap_l;
		if (heap_l->element < heap_r->element)
			return merge_heap_help(heap_l,heap_r);
		else
			return merge_heap_help(heap_r,heap_l);
	}

	friend PBtn merge_heap_help(PBtn heap_l, PBtn heap_r)
	{
		if (0 == heap_l->lchild)
			heap_l->lchild = heap_r;
		else {
			heap_l->rchild = merge_heap(heap_l->rchild,heap_r);
			if (heap_l->lchild->element.npl < heap_l->rchild->element.npl) {
				PBtn ptm = heap_l->lchild;
				heap_l->lchild = heap_l->rchild;
				heap_l->rchild = ptm;
			}
			heap_l->element.npl = heap_l->rchild->element.npl + 1;
		}
		return heap_l;
	}

	typedef Lheap_node<T> Y;

	virtual void insert(const Y& elem);

	T find_min() const;
	T erase_min() const;

	void merge(Left_heap& other_heap);

private:
	//don't support the inefficient finding, erasing and printing operation
	virtual PBtn find(const Y& elem) const { return 0; }
	virtual void erase(const Y& elem) {}
	virtual void print(std::ostream& output) const {}
};

template<typename T>
void Left_heap<T>::insert(const Y& elem)
{
	PBtn ptm = new Bintree_node<Y >(elem);
	hb_root = merge_heap(hb_root, ptm);
}

template<typename T>
void Left_heap<T>::merge(Left_heap& other_heap)
{
	hb_root = merge_heap(hb_root,other_heap.hb_root);
	other_heap.hb_root = 0;
	other_heap.hb_size = 0;
}

template<typename T>
T Left_heap<T>::find_min() const
{
	return hb_root->element;
}

template<typename T>
T Left_heap<T>::erase_min() const
{
	PBtn leftchild = hb_root->lchild;
	PBtn rightchild = hb_root->rchild;
	delete hb_root;
	hb_root = merge_heap(leftchild,rightchild);
}

#endif