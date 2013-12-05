/*******************************************************************************
*
* FileName : sorting.h
* Comment  : some internal sorting algorithms
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-27 23:03
*
*******************************************************************************/

#ifndef _SORTING_H_INCLUDED
#define _SORTING_H_INCLUDED

//-------------insertion sort-------------------------
template<typename T>
void insertion_sort(T a[], int n)
{
	T tm;
	for (int i=0; i!=n; ++i) {
		tm = a[i];
		int k=i;
		for (; k>0; --k) {
			if (tm >= a[k-1])
				break;
			a[k] = a[k-1];
		}
		a[k] = tm;
	}
}
//---------------------------------------------------

//-----------------shell sort------------------------
template<typename T>
void shellsort(T a[], int n)
{
	for (int increment=n/2; increment>=1; increment/=2) {
		if (increment%2 == 0) increment++;
		for (int i=increment; i<n; ++i) {
			T tm = a[i];
			int j=i;
			for (; j>=increment; j-=increment) {
				if (tm >= a[j-increment])
					break;
				a[j] = a[j-increment];
			}
			a[j] = tm;
		}//for
	}//for
}
//----------------------------------------------------

//-----------------heap sort--------------------------
template<typename T>
void percolatedown(T a[], int n, int i)
{
	T tm = a[i];
	int child = i<<1;
	for (; child <= n-1; i=child, child=(child<<1)) {
		//choose the little child
		if (child!= n-1 && a[child] > a[child+1])
			child++;
		if (tm > a[child])
			a[i] = a[child];
		else
			break;
	}
	a[i] = tm;
}

template<typename T>
void heapsort(T a[], int n)
{
	for (int i=n/2; i>=0; --i)
		percolatedown(a,n,i);
	for (int j=n-1; j>=0; --j) {
		T min = a[0];
		a[0] = a[j];	//delete the minimal value
		a[j] = min;
		percolatedown(a,j,0);
	}

	reverse(a,n);
}

template<typename T>
void reverse(T a[], int n)
{
	for (int i=0; i!=n/2; ++i) {
		T tm = a[i];
		a[i] = a[n-1-i];
		a[n-1-i] = tm;
	}
}
//--------------------------------------------------

//----------------merge sort------------------------

template<typename T>
void merge_array(T a[], int beg, int end)
{
	int center = (beg + end) / 2;
	for (int i=beg, j=center+1; i<=end && j<=end;) {
		if (a[i] > a[j]) {
			T tm = a[j];
			for(int k=j; k!=i; --k)
				a[k] = a[k-1];
			a[i] = tm;
			j++;
			i++;
		}
		else
			i++;
	}
}

template<typename T>
void m_sort(T a[], int left, int right)
{
	if (left < right) {
		int center = (left + right) / 2;
		m_sort(a,left,center);
		m_sort(a,center+1,right);
		merge_array(a,left,right);
	}
}

template<typename T>
void mergesort(T a[], int n)
{
	m_sort(a,0,n-1);
}
//--------------------------------------------------

//------------------quick sort----------------------
template<typename T>
T median(T a[], int left, int right)
{
	int center = (left + right) / 2;
	T tm;
	if (a[left] > a[center]) {
		tm = a[left];
		a[left] = a[center];
		a[center] = tm;
 	}
	if (a[left] > a[right]) {
		tm = a[left];
		a[left] = a[right];
		a[right] = tm;
	}
	if (a[center] > a[right]) {
		tm = a[center];
		a[center] = a[right];
		a[right] = tm;
	}

	tm = a[center];
	a[center] = a[right-1];
	a[right-1] = tm;

	return a[right-1];
}

template<typename T>
void q_sort(T a[], int left, int right)
{
	//if the array size is little than 5, use insertion sort
	if (left+4 <= right) {
		T pivot = median(a,left,right);
		int i = left;
		int j = right - 1;
		while (true) {
			while (a[++i] < a[pivot]);
			while (a[--j] > a[pivot]);
			if (i < j) {
				T tm = a[i];
				a[i] = a[j];
				a[j] = tm;
			}
			else
				break;
		}//while
		T tm = a[i];
		a[i] = a[right-1];
		a[right-1] = tm;

		q_sort(a,left,i-1);
		q_sort(a,i+1,right);
	}
	else
		insertion_sort(a+left,right-left+1);
}

template<typename T>
void quicksort(T a[], int n)
{
	q_sort(a,0,n-1);
}
//--------------------------------------------------

#endif