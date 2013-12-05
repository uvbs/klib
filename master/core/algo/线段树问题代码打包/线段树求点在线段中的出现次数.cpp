#include<iostream>
using namespace std;

/* 线段树求点在已知线段中的出现次数 */
#define BORDER 100  // 设线段端点坐标不超过100

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

/* 插入线段[start,end]到线段树, 同时计数区间次数 */
void insert(int index, int start, int end)
{
	if(segTree[index].left == start && segTree[index].right == end)
	{
		++segTree[index].counter;
		return;
	}
	int mid = (segTree[index].left + segTree[index].right) >> 1;
	if(end <= mid)
	{
		insert((index<<1)+1, start, end);
	}else if(start > mid)
	{
		insert((index<<1)+2, start, end);
	}else
	{
		insert((index<<1)+1, start, mid);
		insert((index<<1)+2, mid+1, end);
	}
}

/* 查询点x的出现次数 
 * 从根节点开始到[x,x]叶子的这条路径中所有点计数相加方为x出现次数
 */
int query(int index, int x)
{
	if(segTree[index].left == segTree[index].right) // 走到叶子，返回
	{
		return segTree[index].counter;
	}
	int mid = (segTree[index].left+segTree[index].right) >> 1;
	if(x <= mid)
	{
		return segTree[index].counter + query((index<<1)+1,x);
	}
	return segTree[index].counter + query((index<<1)+2,x);
}

/* 测试线段 */
int segment[10][2] = {
	5, 8,	10, 45,	   0, 7,
	2, 3,	 3, 9,	  13, 26,
   15, 38,  50, 67,	   39, 42, 
   70, 80 
};
/* 测试点 ：answer: 1,2,2,3,1,3,2,1,0,0 */
int testPoint[10] = {
	1, 2, 4, 6, 9,
  15, 13, 44, 48, 90
};

void main()
{
	construct(0,0,100);           // 构建[0,100]线段树
	for(int i = 0; i < 10; ++i)   // 插入测试线段
	{
		insert(0,segment[i][0],segment[i][1]);
	}
	for(int i = 0; i < 10; ++i)   // 查询点出现次数
	{
		printf("frequent of point %d is: %d\n", 
			   testPoint[i], query(0,testPoint[i]));
	}
}