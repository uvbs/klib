#include <iostream>
using namespace std;

/* 线段树求区间最大值 */
#define MAXN 100   /* 最大元素个数 */
#define MAX(a,b) ((a) > (b) ? (a) : (b))

struct Node
{
	int left;
	int right;
	int maxV;
}segTree[4*MAXN];  /* 线段树 */

int arr[] = {56,23,10,78,9,
			100,26,52,30,18};   /* 测试数组 */ 

/* 根据测试数组建立线段树 根节点开始构造[left,right]区间最大值*/
void construct(int index, int left, int right)
{
	segTree[index].left = left;
	segTree[index].right = right;
	if(left == right)  // 叶子，即为该区间最大值，构造结束
	{
		segTree[index].maxV = arr[left];
		return;
	}
	int mid = (left + right) >> 1;
	construct((index<<1)+1, left, mid);
	construct((index<<1)+2, mid+1, right);
	segTree[index].maxV = MAX(segTree[(index<<1)+1].maxV,segTree[(index<<1)+2].maxV);
}

/* 线段树从根节点查询某个区间的最大值 */
int query(int index, int left, int right)
{
	if(segTree[index].left == left && segTree[index].right == right)
	{
		return segTree[index].maxV;
	}
	int mid = (segTree[index].left+segTree[index].right) >> 1;
	if(right <= mid)     // 注：此区间为[0,mid] 故 <=
	{
		return query((index<<1)+1,left,right);
	}else if(left > mid) // 注：此区间为[mid+1,right] 故 >
	{
		return query((index<<1)+2,left,right);
	}
	return MAX(query((index<<1)+1,left,mid), query((index<<1)+2,mid+1,right));
}

void main()
{
	construct(0,0,sizeof arr / sizeof(int) - 1);

	// 注：construct和query函数省略了边界检查

	printf("max of [0,9] is: %d\n",query(0,0,9));
	printf("max of [1,4] is: %d\n",query(0,1,4));
	printf("max of [3,7] is: %d\n",query(0,3,7));
	printf("max of [6,9] is: %d\n",query(0,6,9));
}