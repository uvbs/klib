/*******************************************************************************
*
* FileName : splay_tree.h
* Comment  : splay tree
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-10-10 9:34
*
*******************************************************************************/

#ifndef _SPLAY_TREE_H_INCLUDED
#define _SPLAY_TREE_H_INCLUDED

#include "../../binary_tree/binary_tree/binary_tree.h"
#include "../../Auto_ptr/Auto_ptr/Auto_ptr.h"

namespace {	//anonymous space
template<typename T>
class Splay_tree {
public:
	typedef Bintree_node<T>* PBtn;

	Splay_tree();
	~Splay_tree();

	void clear();

	int depth() const;
	int size() const;

	PBtn find_min(PBtn pTn=0) const;
	PBtn find_max(PBtn pTn=0) const;

	PBtn find(const T& elem) const;
	void insert(const T& elem);
	void erase(const T& elem);
	void splay(const T& elem);
	void print_tree(std::ostream& output) const;

	PBtn single_rotate_with_left(PBtn pTn);
	PBtn single_rotate_with_right(PBtn pTn);

private:
	PBtn insert(const T& elem, PBtn pTn);
	PBtn erase(const T& elem, PBtn pTn);
	PBtn splay(const T& elem, PBtn pTn);
	void print_tree(std::ostream& output, PBtn pTn, int depth) const;

	int depth(const PBtn pTn) const;
	void destory(PBtn pTn);

	static PBtn null_node;
	PBtn hb_root;
	int hb_size;
};

template<typename T>
typename Splay_tree<T>::PBtn Splay_tree<T>::null_node = 0;

template<typename T>
Splay_tree<T>::Splay_tree() : hb_root(0), hb_size(0)
{
	if (0 == null_node) {
		null_node = new Bintree_node<T>;
		null_node->lchild = null_node->rchild = null_node;
	}
	hb_root = null_node;
}

template<typename T>
Splay_tree<T>::~Splay_tree()
{
	clear();
}

template<typename T>
inline void Splay_tree<T>::clear()
{
	if (null_node == hb_root)
		assert(0 == hb_size);
	else {
		destory(hb_root);
	}
}

template<typename T>
void Splay_tree<T>::destory(PBtn pTn)
{
	if (null_node == pTn)
		return;
	PBtn ptml=pTn->lchild;
	PBtn ptmr=pTn->rchild;
	delete pTn;
	destory(ptml);
	destory(ptmr);
}

template<typename T>
inline int Splay_tree<T>::depth() const
{
	return depth(hb_root);
}

template<typename T>
int Splay_tree<T>::depth(const PBtn pTn) const
{
	if (null_node == pTn)
		return 0;
	else {
		int ldepth=depth(pTn->lchild);
		int rdepth=depth(pTn->rchild);
		return 1 + (ldepth>=rdepth ? ldepth : rdepth);
	}
}

template<typename T>
inline int Splay_tree<T>::size() const
{
	return hb_size;
}

template<typename T>
inline typename Splay_tree<T>::PBtn Splay_tree<T>::find_min(
	PBtn pTn=0) const
{
	if (0 == pTn)
		pTn = hb_root;
	if (null_node == pTn)
		return 0;
	while (pTn->lchild != null_node)
		pTn=pTn->lchild;
	return pTn;
}

template<typename T>
inline typename Splay_tree<T>::PBtn Splay_tree<T>::find_max(
	PBtn pTn=0) const
{
	if (0 == pTn)
		pTn = hb_root;
	if (null_node == pTn)
		return 0;
	while (pTn->rchild != null_node)
		pTn=pTn->rchild;
	return pTn;
}

template<typename T>
typename Splay_tree<T>::PBtn Splay_tree<T>::find(const T& elem) const
{
	PBtn pTn=hb_root;
	while (pTn != null_node) {
		if (elem == pTn->element)
			return pTn;
		if (elem < pTn->element)
			pTn=pTn->lchild;
		else
			pTn=pTn->rchild;
	}
	return 0;	//can not find
}

template<typename T>
inline typename Splay_tree<T>::PBtn 
Splay_tree<T>::single_rotate_with_left(PBtn pTn)
{
	PBtn ptm=pTn->lchild;
	pTn->lchild=ptm->rchild;
	ptm->rchild=pTn;
	return ptm;
}

template<typename T>
inline typename Splay_tree<T>::PBtn 
Splay_tree<T>::single_rotate_with_right(PBtn pTn)
{
	PBtn ptm=pTn->rchild;
	pTn->rchild=ptm->lchild;
	ptm->lchild=pTn;
	return ptm;
}

template<typename T>
inline void Splay_tree<T>::insert(const T& elem)
{
	hb_root = insert(elem, hb_root);
}

template<typename T>
inline void Splay_tree<T>::erase(const T& elem)
{
	hb_root = erase(elem, hb_root);
}

template<typename T>
inline void Splay_tree<T>::splay(const T& elem)
{
	hb_root = splay(elem, hb_root);
}

template<typename T>
typename Splay_tree<T>::PBtn Splay_tree<T>::insert(const T& elem, PBtn pTn)
{
	PBtn new_node = new Bintree_node<T>;
	new_node->element = elem;

	if (pTn == null_node) {
		new_node->lchild = new_node->rchild = null_node;
		pTn = new_node;
		++hb_size;
	}
	else {
		pTn = splay(elem, pTn);
		if (elem < pTn->element) {
			new_node->lchild = pTn->lchild;
			new_node->rchild = pTn;
			pTn->lchild = null_node;
			pTn = new_node;
			++hb_size;
		}
		else if (elem > pTn->element) {
			new_node->rchild = pTn->rchild;
			new_node->lchild = pTn;
			pTn->rchild = null_node;
			pTn = new_node;
			++hb_size;
		}
		else { //elem == pTn->element
			delete new_node;
		}
	}
	return pTn;
}

template<typename T>
typename Splay_tree<T>::PBtn Splay_tree<T>::erase(const T& elem, PBtn pTn)
{
	if (pTn == null_node) {
		std::cerr<<"error, the splay tree is empty!\n";
		return pTn;
	}

	pTn = splay(elem,pTn);

	if (elem != pTn->element) {
		std::cerr<<"error, the element not exist!\n";
		return pTn;
	}

	PBtn ptm;
	if (pTn->lchild == null_node) {
		ptm = pTn->rchild;
	}
	else {
		ptm = pTn->lchild;
		ptm = splay(elem,ptm);
		ptm->rchild = pTn->rchild;
	}
	delete pTn;
	--hb_size;
	pTn = ptm;
	return pTn;
}

template<typename T>
typename Splay_tree<T>::PBtn Splay_tree<T>::splay(const T& elem, PBtn pTn)
{
	Auto_ptr<Bintree_node<T> > holder(new Bintree_node<T>);
	holder->lchild = holder->rchild = 0;
	PBtn left_max = holder.get();
	PBtn right_min = holder.get();
	null_node->element = elem;

	while (elem != pTn->element) {
		if (elem < pTn->element) {
			if (elem < pTn->lchild->element)
				pTn = single_rotate_with_left(pTn);
			if (pTn->lchild == null_node)
				break;
			right_min->lchild = pTn;
			right_min = pTn;
			pTn = pTn->lchild;
		}
		else {
			if (elem > pTn->rchild->element)
				pTn = single_rotate_with_right(pTn);
			if (pTn->rchild == null_node)
				break;
			left_max->rchild = pTn;
			left_max = pTn;
			pTn = pTn->rchild;
		}
	}//while
	left_max->rchild = pTn->lchild;
	right_min->lchild = pTn->rchild;
	pTn->lchild = holder->rchild;
	pTn->rchild = holder->lchild;
	return pTn;
}

template<typename T>
inline void Splay_tree<T>::print_tree(std::ostream& output) const
{
	output<<"--------------------------------------------\n";
	print_tree(output,hb_root,1);
	output<<"--------------------------------------------\n";
}

template<typename T>
void Splay_tree<T>::print_tree(std::ostream& output, 
								PBtn pTn, int depth) const
{
	if (null_node == pTn)
		return;
	print_tree(output,pTn->lchild,depth+1);
	for (int i=0; i!=depth; ++i)
		output<<"    ";
	output<<pTn->element<<'\n';
	print_tree(output,pTn->rchild,depth+1);
}

}	//anonymous namespace

#endif