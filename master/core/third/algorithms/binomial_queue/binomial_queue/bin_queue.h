/*******************************************************************************
*
* FileName : bin_queue.h
* Comment  : binomial queue
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-27 0:14
*
*******************************************************************************/

#ifndef _BIN_QUEUE_H_INCLUDED
#define _BIN_QUEUE_H_INCLUDED

#include <vector>

template<typename T>
struct Bin_node {
	T element;
	Bin_node* firstchild;
	Bin_node* nextsibling;

	Bin_node(T elem=T()) : element(elem), firstchild(0), nextsibling(0) {}
};

template<typename T>
class Bin_queue {
public:
	Bin_queue(int array_size);
	~Bin_queue();

	int size() const { return hb_size; }
	void clear();

	void insert(const T elem);
	T find_min() const;
	T erase_min();
	void merge(Bin_queue& other);

private:
	int hb_size;
	int hb_capacity;
	int hb_array_size;
	std::vector<Bin_node<T>*> hb_tree;

	Bin_node<T>* combine_tree(Bin_node<T>* lhs, Bin_node<T>* rhs);
	void destory(Bin_node<T>* pBn);
};

template<typename T>
Bin_queue<T>::Bin_queue(int array_size) : hb_array_size(array_size), hb_size(0)
{
	hb_capacity = (1<<array_size) - 1;
	hb_tree.resize(hb_array_size);
	for (int i=0; i!=hb_array_size; ++i)
		hb_tree[i] = 0;
}

template<typename T>
Bin_queue<T>::~Bin_queue()
{
	clear();
	hb_size = 0;
}

template<typename T>
void Bin_queue<T>::clear()
{
	for (int i=0; i!=hb_array_size; ++i) {
		destory(hb_tree[i]);
		hb_tree[i] = 0;
	}
}

template<typename T>
void Bin_queue<T>::destory(Bin_node<T> *pBn)
{
	if (0 == pBn)
		return;
	destory(pBn->firstchild);
	destory(pBn->nextsibling);
	delete pBn;
}

template<typename T>
void Bin_queue<T>::insert(const T elem)
{
	Bin_node<T>* pnew = new Bin_node<T>(elem);
	Bin_queue<T> tm_bq(hb_array_size);
	tm_bq.hb_tree[0] = pnew;
	tm_bq.hb_size = 1;
	merge(tm_bq);
}

template<typename T>
T Bin_queue<T>::find_min() const
{
	if (0 == hb_size) {
		std::cerr<<"error, the binomial queue is empty!\n";
		return T();
	}
	T tm = hb_tree[0]->element;
	for (int i=0; i!=hb_arrary_size; ++i) {
		if (hb_tree[i]->element < tm)
			tm = hb_tree[i];
	}
	return tm;
}

template<typename T>
T Bin_queue<T>::erase_min()
{
	if (0 == hb_size) {
		std::cerr<<"error, the binomial queue is empty!\n";
		return T();
	}

	int k=0;
	while (0 == hb_tree[k]) {
		++k;
	}
	T tm = hb_tree[k]->element;
	int min=k;
	for (int i=0; i!=hb_array_size; ++i) {
		if (hb_tree[i]!=0 && hb_tree[i]->element < tm) {
			tm = hb_tree[i]->element;
			min = i;
		}//if
	}

	Bin_node<T>* oldroot = hb_tree[min];
	Bin_node<T>* ptm = oldroot->firstchild;
	delete oldroot;
	hb_tree[min] = 0;
	hb_size -= 1<<min;

	Bin_queue<T> tm_bq(hb_array_size);
	tm_bq.hb_size = (1<<min) - 1;
	for (int j=min-1; j>=0; --j) {
		tm_bq.hb_tree[j] = ptm;
		ptm = ptm->nextsibling;
		tm_bq.hb_tree[j]->nextsibling = 0;
	}
	merge(tm_bq);
	return tm;
}

template<typename T>
void Bin_queue<T>::merge(Bin_queue<T>& other)
{
	if (hb_size + other.hb_size > hb_capacity) {
		std::cerr<<"error, the binomial should exceed capacity!\n";
		return;
	}
	hb_size += other.hb_size;
	Bin_node<T>* ptm = 0;
	Bin_node<T>* ptmo = 0;
	Bin_node<T>* pBn = 0;
	for (int i=0; i!=hb_array_size; ++i) {
		ptm = hb_tree[i];
		ptmo = other.hb_tree[i];

		switch (!!ptm + (!!ptmo<<1) + (!!pBn<<2)) {
			case 0 :	//ptm=ptmo=pBn=0
				break;
			case 1 :	//ptmo=pBn=0
				break;
			case 2 :	//ptm=pBn=0
				hb_tree[i] = ptmo;
				other.hb_tree[i] = 0;
				break;
			case 3 :	//pBn=0
				pBn = combine_tree(ptm,ptmo);
				hb_tree[i] = other.hb_tree[i] = 0;
				break;
			case 4 :	//ptm=ptmo=0
				hb_tree[i] = pBn;
				pBn = 0;
				break;
			case 5 :	//ptmo=0
				pBn = combine_tree(ptm,pBn);
				hb_tree[i] = 0;
				break;
			case 6 :	//ptm=0
				pBn = combine_tree(ptmo,pBn);
				other.hb_tree[i] = 0;
				break;
			case 7 :	//all!=0
				hb_tree[i] = pBn;
				pBn = combine_tree(ptm,ptmo);
				other.hb_tree[i] = 0;
				break;
		}//switch
	}
	other.hb_size = 0;
}

//combine the two binomial trees which have the same depth
template<typename T>
Bin_node<T>* Bin_queue<T>::combine_tree(Bin_node<T>* lhs, Bin_node<T>* rhs)
{
	if (lhs->element < rhs->element) {
		rhs->nextsibling = lhs->firstchild;
		lhs->firstchild = rhs;
		return lhs;
	}
	else {
		lhs->nextsibling = rhs->firstchild;
		rhs->firstchild = lhs;
		return rhs;
	}
}

#endif