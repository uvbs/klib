#include<iostream>
using namespace std;

/* 线段树求线段覆盖长度 */
#define BORDER 100  // 设线段端点坐标不超过100

struct Node         // 线段树
{
	int left;
	int right;
	int isCover;    // 标记是否被覆盖
}segTree[4*BORDER];  

/* 构建线段树 根节点开始构建区间[lef,rig]的线段树*/
void construct(int index, int lef, int rig)
{
	segTree[index].left = lef;
	segTree[index].right = rig;
	if(rig - 1 == lef)                 // 到单位1线段
	{
		segTree[index].isCover = 0;
		return;
	}
	int mid = (lef+rig) >> 1;
	construct((index<<1)+1, lef, mid);
	construct((index<<1)+2, mid, rig); // 非mid+1，线段覆盖[mid,mid+1]
	segTree[index].isCover = 0;
}

/* 插入线段[start,end]到线段树, 同时标记覆盖 */
void insert(int index, int start, int end)
{
	if(segTree[index].isCover == 1)	return; // 如已覆盖，没必要继续向下插

	if(segTree[index].left == start && segTree[index].right == end)
	{
		segTree[index].isCover = 1;
		return;
	}
	int mid = (segTree[index].left + segTree[index].right) >> 1;
	if(end <= mid)
	{
		insert((index<<1)+1, start, end);
	}else if(start >= mid)             // 勿漏=
	{
		insert((index<<1)+2, start, end);
	}else
	{
		insert((index<<1)+1, start, mid);
		insert((index<<1)+2, mid, end);  
		// 注：不是mid+1，线段覆盖，不能漏[mid,mid+1]
	}
}

/* 计算线段覆盖长度 */
int Count(int index)
{
	if(segTree[index].isCover == 1)
	{
		return segTree[index].right - segTree[index].left;
	}else if(segTree[index].right - segTree[index].left == 1)
	{
		return 0;
	}
	return Count((index<<1)+1) + Count((index<<1)+2);
}

/* 测试线段 answer: 71 */
int segment[10][2] = {
	5, 8,	10, 45,	   0, 7,
	2, 3,	 3, 9,	  13, 26,
   15, 38,  50, 67,	   39, 42, 
   70, 80 
};

void main()
{
	construct(0,0,100);           // 构建[0,100]线段树
	for(int i = 0; i < 10; ++i)   // 插入测试线段
	{
		insert(0,segment[i][0],segment[i][1]);
	}
	printf("the cover length is %d\n", Count(0));
}