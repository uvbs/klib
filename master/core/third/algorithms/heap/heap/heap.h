/*******************************************************************************
*
* FileName : heap.h
* Comment  : heap base class
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-25 1:07
*
*******************************************************************************/

#ifndef _HEAP_H_INCLUDED
#define _HEAP_H_INCLUDED

#include <vector>
#include <ostream>

template<typename T>
class Heap {
public:
	explicit Heap(int max_size, const T limit);
	~Heap();

	bool empty() const;
	bool full() const;
	int size() const { return hb_size; }
	void print(std::ostream& output) const;
	void clear();

	virtual void insert(const T& elem) = 0;
	virtual void decrease_key(int p, T val) = 0;
	virtual void increase_key(int p, T val) = 0;

protected:
	int hb_capacity;
	int hb_size;
	std::vector<T> hb_element;
};

template<typename T>
Heap<T>::Heap(int max_size, const T limit) 
			: hb_capacity(max_size), hb_size(0)
{
	hb_element.resize(hb_capacity);
	hb_element[0] = limit;
}

template<typename T>
Heap<T>::~Heap()
{
}

template<typename T>
inline bool Heap<T>::empty() const
{
	return hb_size==0 ? true : false;
}

template<typename T>
inline bool Heap<T>::full() const
{
	return hb_size==hb_capacity ? true : false;
}

template<typename T>
void Heap<T>::print(std::ostream& output) const
{
	if (empty())
		return;
	output<<"----------------------------------------------------\n";
	int i=1;
	while(true) {
		int temp=i;
		while (temp != i<<1) {
			output<<hb_element[temp++]<<"  ";
			if (temp > hb_size)
				break;
		}
		output<<'\n';
		if (temp > hb_size)
			break;
		i = i<<1;
	}
	output<<"----------------------------------------------------\n";
}

template<typename T>
inline void Heap<T>::clear()
{
	delete hb_element;
	hb_size = 0;
}

#endif