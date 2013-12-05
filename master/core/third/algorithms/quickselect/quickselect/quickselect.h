/*******************************************************************************
*
* FileName : quickselect.h
* Comment  : quick select
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-28 11:30
*
*******************************************************************************/

#ifndef _QUICKSELECT_H_INCLUDED
#define _QUICKSELECT_H_INCLUDED

#include "../../sorting/sorting/sorting.h"

template<typename T>
void quickselect(T a[], int n, int k)
{
	q_select(a,0,n-1,k);
}

template<typename T>
void q_select(T a[], int left, int right, int k)
{
	if (left + 4 <= right) {
		T pivot = median(a,left,right);
		int i = left;
		int j = right - 1;
		while (true) {
			while (a[++i] < pivot);
			while (a[--j] > pivot);
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

		if (k <= i)
			q_select(a,left,i-1,k);
		else if (k > i+1)
			q_select(a,i+1,right,k);
		else	//i == k-1, find the k-th smallest element
			return;
	}
	else
		insertion_sort(a+left, right-left+1);
}

#endif