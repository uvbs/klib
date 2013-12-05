#include<iostream>
using namespace std;

/* 线段树求逆序数 */
#define BORDER 100  // 设测试数列最大不超过100

struct Node         // 线段树
{
	int left;
	int right;
	int counter;
}segTree[4*BORDER];  

/* 构建线段树 根节点开始构建区间[lef,rig]的线段树*/
void construct(int index, int lef, int rig)
{
	segTree[index].left = lef;
	segTree[index].right = rig;
	if(lef == rig)   // 叶节点
	{
		segTree[index].counter = 0;
		return;
	}
	int mid = (lef+rig) >> 1;
	construct((index<<1)+1, lef, mid);
	construct((index<<1)+2, mid+1, rig);
	segTree[index].counter = 0;
}

/* 包含x的区间的值都+1 即对包含x的logn个节点的路径进行更新*/
void insert(int index, int x)
{
	++segTree[index].counter;
	if(segTree[index].left == segTree[index].right)
	{
		return;
	}
	int mid = (segTree[index].left + segTree[index].right) >> 1;
	if(x <= mid)
	{
		insert((index<<1)+1, x);
	}else
	{
		insert((index<<1)+2, x);
	}
}

/* 查询点x的逆序数，即查询[x,BORDER]的区间和 */
int query(int index, int left, int right)
{
	if(segTree[index].left == left && segTree[index].right == right)
	{
		return segTree[index].counter;
	}
	int mid = (segTree[index].left+segTree[index].right) >> 1;
	if(right <= mid)     
	{
		return query((index<<1)+1,left,right);
	}else if(left > mid)
	{
		return query((index<<1)+2,left,right);
	}
	return query((index<<1)+1,left,mid) + query((index<<1)+2,mid+1,right);
}

/* 测试序列 ：answer: 10 */
int testPoint[10] = {
	1, 4, 2, 9, 48,
   15, 13, 44, 6, 90
};

void main()
{
	construct(0,0,100);           // 构建[0,100]线段树
	int reverseSum = 0;           // 记录逆序对数
	for(int i = 0; i < 10; ++i)   // 查询当前逆序数
	{
		reverseSum += query(0,testPoint[i],BORDER);
		printf("num of reverse order is: %d\n", reverseSum);
		insert(0,testPoint[i]);
	}
}