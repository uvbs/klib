/*******************************************************************************
*
* FileName : binary_tree.h
* Comment  : binary tree, just as base class
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-23 12:46
*
*******************************************************************************/

#ifndef _BINARY_TREE_H_INCLUDED
#define _BINARY_TREE_H_INCLUDED

#include <cassert>
#include <ostream>

template<typename T>
struct Bintree_node {
	T element;
	Bintree_node* lchild;
	Bintree_node* rchild;

	explicit Bintree_node(const T& elem=T(), 
		Bintree_node* lchd=0, Bintree_node* rchd=0)
		: element(elem), lchild(lchd), rchild(rchd) {}
};

template<typename T>
class Binary_tree {
public:
	typedef Bintree_node<T>* PBtn;
	Binary_tree();
	virtual ~Binary_tree();

	void clear();

	int depth() const;
	int size() const;

	virtual PBtn find(const T& elem) const=0;
	virtual void insert(const T& elem)=0;
	virtual void erase(const T& elem)=0;
	virtual void print(std::ostream& output) const=0;
	void print_tree(std::ostream& output) const;

protected:
	int depth(const PBtn pTn) const;
	void destory(PBtn pTn);

	PBtn hb_root;
	int hb_size;

private:
	void print_tree(std::ostream& output, PBtn pTn, int depth) const;
};

template<typename T>
Binary_tree<T>::Binary_tree() : hb_root(0), hb_size(0)
{
}

template<typename T>
Binary_tree<T>::~Binary_tree()
{
	clear();
}

template<typename T>
inline void Binary_tree<T>::clear()
{
	if (0 == hb_root)
		assert(0 == hb_size);
	else {
		destory(hb_root);
	}
}

template<typename T>
void Binary_tree<T>::destory(PBtn pTn)
{
	if (0 == pTn)
		return;
	PBtn ptml=pTn->lchild;
	PBtn ptmr=pTn->rchild;
	delete pTn;
	destory(ptml);
	destory(ptmr);
}

template<typename T>
inline int Binary_tree<T>::depth() const
{
	return depth(hb_root);
}

template<typename T>
int Binary_tree<T>::depth(const PBtn pTn) const
{
	if (0 == pTn)
		return 0;
	else {
		int ldepth=depth(pTn->lchild);
		int rdepth=depth(pTn->rchild);
		return 1 + (ldepth>=rdepth ? ldepth : rdepth);
	}
}

template<typename T>
inline int Binary_tree<T>::size() const
{
	return hb_size;
}

template<typename T>
inline void Binary_tree<T>::print_tree(std::ostream& output) const
{
	output<<"--------------------------------------------\n";
	print_tree(output,hb_root,1);
	output<<"--------------------------------------------\n";
}

template<typename T>
void Binary_tree<T>::print_tree(std::ostream& output, 
								PBtn pTn, int depth) const
{
	if (0 == pTn)
		return;
	print_tree(output,pTn->lchild,depth+1);
	for (int i=0; i!=depth; ++i)
		output<<"    ";
	output<<pTn->element<<'\n';
	print_tree(output,pTn->rchild,depth+1);
}

#endif