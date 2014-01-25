
#ifndef _XLIB_SORT_H_
#define _XLIB_SORT_H_

namespace klib{
namespace algo{


template<class T>
void Swap(T list[], int l, int r)
{
    T *temp = list + l;
    list[l] = list[r];
    list[r] = *temp;
}

//----------------------------------------------------------------------
// Summary:
//      冒泡排序,时间复杂度 0(N) - 0(N^2)
//----------------------------------------------------------------------
template<class T>
void BubbleSort(T list[], int l, int r)
{
    if(l<r)
    {
        for(i=l;i<r;j++)
        {
            if(list[i]>list[i+1])
            {
                Swap(list,i,i+1);
            }
        }
        BubbleSort(list,l,r-1);
    }
}


//******************************************************快速排序 O(lgN) - 0(N^2) 不隐定排序
/*
设要排序的数组是A[0]……A[N-1]，首先任意选取一个数据（通常选用第一个数据）作为关键数据，然后将所有比它小的数都放到它前面，
所有比它大的数都放到它后面，这个过程称为一趟快速排序。
值得注意的是，快速排序不是一种稳定的排序算法，也就是说，多个相同的值的相对位置也许会在算法结束时产生变动。
一趟快速排序的算法是：
1）设置两个变量I、J，排序开始的时候：I=0，J=N-1；
2）以第一个数组元素作为关键数据，赋值给key，即 key=A[0]；
3）从J开始向前搜索，即由后开始向前搜索（J=J-1），找到第一个小于key的值A[J]，并与key交换；
4）从I开始向后搜索，即由前开始向后搜索（I=I+1），找到第一个大于key的A[I]，与key交换；
5）重复第3、4、5步，直到 I=J； (3,4步是在程序中没找到时候j=j-1，i=i+1，直至找到为止。找到并交换的时候i， j指针位置不变。
另外当i=j这过程一定正好是i+或j-完成的最后另循环结束。）
*/


//一次基数切割排序 返回最后基数位置
template<class T>
int Division(T list[],int l,int r)
{
    T temp;
    while(l<r)
    {
        while(l<r && list[r]>list[l])
            r--;
        Swap(list,l,r);
        while(l<r && list[l]<list[r])
            l++;
        Swap(list,l,r);
    }
    return l;
}

//快速排序
template<class T>
void QuickSort(T list[],int l,int r)
{
    if(l<r)
    {
        int i = Division(list,l,r);
        QuickSort(list,l,i-1);
        QuickSort(list,i+1,r);
    }
}


//***************************************************************直接选择排序 O(N^2) 不隐定排序
template<class T>
void SelSort(T list[],int l,int r)
{
    if(l<r)
    {
        int i,min = l;
        for(i=l+1;i<r;i++)
            if(list[i]<list[min])
                min = i;
        Swap(list,l,min);
        SelSort(list,l+1,r);
    }
}

//****************************************************************堆排序 O(NlogN) 不隐定排序
template<class T>
void HeapAdjust(T list[],int i,int size) //调整堆 
{
    int l=2*i; //i的左孩子节点序号 
    int r=2*i+1; //i的右孩子节点序号 
    int max=i; //临时变量 
    if(i<=size/2) //如果i不是叶节点就进行调整 
    {
        if(l<=size&&list[l]>list[max])
        {
            max=l;
        } 
        if(r<=size&&list[r]>list[max])
        {
            max=r;
        }
        if(max!=i)
        {
            Swap(list,i,max);
            HeapAdjust(list,max,size); //避免调整之后以max为父节点的子树不是堆 
        }
    } 
}

template<class T>
void BuildHeap(T list[],int size) //建立堆 
{
    int i;
    for(i=size/2;i>=1;i--) //非叶节点最大序号值为size/2 
    {
        HeapAdjust(list,i,size);
    }
}

//堆排序 找出前top个最大值
template<class T>
void HeapSort(T list[],int size,int top)
{
    int i;
    BuildHeap(list,size);//建立堆 
    for(i=size;i>=size-top;i--)
    {
        Swap(list,1,i);//交换堆顶和最后一个元素，即每次将剩余元素中的最大者放到最后面 
        HeapAdjust(a,1,i-1);//重新调整堆顶节点成为大顶堆
    }
}

//*************************************************直接插入排序 例如洗牌 O(N^2) 
template<class T>
void InsertSort(T list[],int size)
{
    int i,j;
    for(i=1;i<size;i++)//拿一张牌
    {
        for(j = i-1;j>=0 && list[i]<list[j];j--)//比较手上的已经排好的牌 
            list[j+1] = list[j]; 
        list[j+1] = list[i]; //找到合适位置插入
    }
}


//*************************************************希尔排序 O(N^3/2) - O(N^2) 不稳定排序
/*
先取一个小于n的整数d1作为第一个增量，把文件的全部记录分成d1个组。所有距离为d1的倍数的记录放在同一个组中。
先在各组内进行直接插入排序；然后，取第二个增量d2<d1重复上述的分组和排序，直至所取的增量dt=1(dt<dt-l<…<d2<d1)，
即所有记录放在同一组中进行直接插入排序为止。
*/
template<class T>
void ShellSort(T list[],int size)
{
    int i,j,step = size/2;
    while(step>=1)
    {
        for(i=0;i<size;i++)
        {
            for(j=i-step;j>= && list[i]<list[j];j-=step)
                list[j+step] = list[j];
            list[j+step] = list[i];
        }
        step=step/2;
    }
}

//**************************************************归并排序

// 数组的两两合并操作
template<class T>
void Merger(T list[], T temp[], int l, int mid, int r)
{
    int lEnd = mid-1;//左指针尾
    int rStart = mid+1;//右指针头
    int tempIdx = l;//临时数组的下标

    //先循环两个区间段选择最小值放入临时数组
    while((l<lEnd)&&(rStart<r))
    {
        if(list[l]<list[rStart])
            temp[tempIdx++] = list[l++];
        else
            temp[tempIdx++] = list[rStart++];
    }
    //判断左序列是否结束
    while(l<=lEnd)
        temp[tempIdx++] = list[l++];
    //判断右序列是否结束
    while(rStart<=right)
        temp[tempIdx++] = list[rStart++];
    int i;
    //放入已排好的数据
    for(i=l;i<r;i++)
        list[i] = temp[i];
}

//归并排序
template<class T>
void MergerSort(T list[], T temp[], int l, int r)
{
    if(l<r)
    {
        int mid = (l+r)/2;
        //递归划分数组左序列
        MergerSort(list,temp,l,mid);
        //递归划分数组右序列
        MergerSort(list,temp,mid+1,r);
        //数组合并操作
        Merger(list,l,mid+1,r);
    }
}

/**
* 算法: 交换二对象
**/
template<typename T>
void t_swap( T& v1, T& v2 )
{
    T t = v1; v1 = v2; v2 = t;
}

/**
* 算法: 反转序列
**/
template<typename T>
void t_reverse( T* v, size_t size )
{
    size_t s = 0, e = size-1;
    while( s < e && s < size && e > 0 )
        t_swap( v[s++], v[e--] );
}

/**
* 算法: 手摇算法,从指定位置旋转序列(见编程珠玑第二章)
**/
template<typename T>
void t_exchange( T* v, size_t size, size_t n )
{
    t_reverse( v, n );
    t_reverse( v + n, size - n );
    t_reverse( v, size );
}

/**
* 算法: 合并二已排序的连续序列
**/
template<typename T>
void t_merge( T& v, size_t size, size_t pos )
{
    size_t fir = 0, sec = pos;
    while ( fir < sec && sec < size )
    {
        while ( fir < sec && v[fir] <= v[sec] ) fir++;
        size_t maxMove = 0;
        while ( sec < size && v[fir] > v[sec] ) maxMove++, sec++;
        t_exchange( &v[fir], sec - fir, sec - fir - maxMove );
        fir += maxMove;

        display(array, sizeof(array)/sizeof(int));
    }
}

/**
* 算法: 归并排序
**/
template<typename T>
void t_merge_sort( T* v, size_t size )
{
    if ( size <= 1 ) return;
    t_merge_sort( v, size/2 );
    t_merge_sort( v + size/2, size - size/2 );
    t_merge( v, size, size/2 );
}



}}


#endif //_ADVERTISEMENT_SORT_H_