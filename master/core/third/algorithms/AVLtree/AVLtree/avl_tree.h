/*******************************************************************************
*
* FileName : avl_tree.h
* Comment  : AVL tree
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-23 12:46
*
*******************************************************************************/

#ifndef _AVL_TREE_H_INCLUDED
#define _AVL_TREE_H_INCLUDED

#include "../../bstree/bstree/bstree.h"

template<typename T>
class Avl_tree : public Bstree<T> {
public:
	void insert(const T& elem);
	void erase(const T& elem);

	PBtn single_rotate_with_left(PBtn pTn);
	PBtn single_rotate_with_right(PBtn pTn);
	PBtn double_rotate_with_left(PBtn pTn);
	PBtn double_rotate_with_right(PBtn pTn);

private:
	PBtn insert(const T& elem, PBtn pTn);
	PBtn erase(const T& elem, PBtn pTn);
};

template<typename T>
inline typename Avl_tree<T>::PBtn 
Avl_tree<T>::single_rotate_with_left(PBtn pTn)
{
	PBtn ptm=pTn->lchild;
	pTn->lchild=ptm->rchild;
	ptm->rchild=pTn;
	return ptm;
}

template<typename T>
inline typename Avl_tree<T>::PBtn 
Avl_tree<T>::single_rotate_with_right(PBtn pTn)
{
	PBtn ptm=pTn->rchild;
	pTn->rchild=ptm->lchild;
	ptm->lchild=pTn;
	return ptm;
}

template<typename T>
inline typename Avl_tree<T>::PBtn 
Avl_tree<T>::double_rotate_with_left(PBtn pTn)
{
	pTn->lchild=single_rotate_with_right(pTn->lchild);
	return single_rotate_with_left(pTn);
}

template<typename T>
inline typename Avl_tree<T>::PBtn 
Avl_tree<T>::double_rotate_with_right(PBtn pTn)
{
	pTn->rchild=single_rotate_with_left(pTn->rchild);
	return single_rotate_with_right(pTn);
}

template<typename T>
inline void Avl_tree<T>::insert(const T& elem)
{
	hb_root=insert(elem,hb_root);
}

template<typename T>
inline void Avl_tree<T>::erase(const T& elem)
{
	hb_root=erase(elem,hb_root);
}

template<typename T>
typename Avl_tree<T>::PBtn Avl_tree<T>::insert(const T& elem, PBtn pTn)
{
	if (0 == pTn) {
		pTn=new Bintree_node<T>(elem,0,0);
		++hb_size;
	}
	else {
		if (elem == pTn->element)
			return pTn;
		else if (elem < pTn->element) {
			pTn->lchild = insert(elem,pTn->lchild);
			if (2 == depth(pTn->lchild) - depth(pTn->rchild)) {
				if (elem < pTn->lchild->element)
					pTn=single_rotate_with_left(pTn);
				else
					pTn=double_rotate_with_left(pTn);
			}
		}//else if
		else {	//elem > pTn->element
			pTn->rchild = insert(elem,pTn->rchild);
			if (2 == depth(pTn->rchild) - depth(pTn->lchild)) {
				if (elem > pTn->rchild->element)
					pTn=single_rotate_with_right(pTn);
				else
					pTn=double_rotate_with_right(pTn);
			}
		}//else
	}//else
	return pTn;
}

template<typename T>
typename Avl_tree<T>::PBtn Avl_tree<T>::erase(const T& elem, PBtn pTn)
{
	if (0 == pTn)
		std::cerr<<"error, the element not exist!\n";
	else {
		if (elem < pTn->element) {
			pTn->lchild = erase(elem,pTn->lchild);
			if (2 == depth(pTn->rchild) - depth(pTn->lchild)) {
				if (depth(pTn->rchild->rchild) > depth(pTn->rchild->lchild))
					pTn=single_rotate_with_right(pTn);
				else
					pTn=double_rotate_with_right(pTn);
			}
		}
		else if (elem > pTn->element) {
			pTn->rchild = erase(elem,pTn->rchild);
			if (2 == depth(pTn->lchild) - depth(pTn->rchild)) {
				if (depth(pTn->lchild->lchild) > depth(pTn->lchild->rchild))
					pTn=single_rotate_with_left(pTn);
				else
					pTn=double_rotate_with_left(pTn);
			}
		}
		else { //elem == pTn->element
			if (pTn->lchild!=0 && pTn->rchild!=0) {
				PBtn ptm = delete_min(pTn->rchild);
				if (ptm == pTn->rchild)
					pTn->rchild=ptm->rchild;
				pTn->element = ptm->element;
				delete ptm;
				--hb_size;
			}
			else {
				PBtn ptm=pTn;
				if (0 == pTn->lchild)
					pTn = pTn->rchild;
				else  //pTn->rchild == 0
					pTn = pTn->lchild;
				delete ptm;
				--hb_size;
			}
		}//end of else(elem == pTn->element)
	}//else
	return pTn;
}

#endif