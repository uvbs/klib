
#ifndef _Klib_SEARCH_H_
#define _Klib_SEARCH_H_

namespace klib {
namespace algo {



//***********************************顺序查找算法 排列从low-high的序列 平均查找长度：ASL = (n+1)/2
template<class T>
int SeqSearch(T list[],int count,T key)
{
    int i;
    for(i=count ; list[i] == key ; i--);
    return i;
}

//***********************************折半查找
template<class T>
int BinSearch(T list[],int count,T key)
{
    int mid, low = 0;
    int high = count - 1;//查找区间的上中下界
    while (low <= high)
    {
        mid = (low+high)/2;
        if(e == list[mid]) {
            return mid;
        }
        else if(e < list[mid]) {
            high = mid-1;
        }
        else {
            low = mid+1;
        }
    }

    return -1;
}



//***********************************分块查找

//索引块
template<class T>
class TIdx
{
    T key;//块中最大值（最小值）
    int index;//对应到线性表中索引
    int size;//块大小
};

template<class T>
int IdxSearch(T list[], int count,T key, TIdx<T> idx[],int idxcount)
{
    //二分法查找块
    int mid, low = 0;
    int high = idxcount - 1;
    while(low<=high)
    {
        mid = (low+high)/2;
        if(e == list[mid]) {
            return mid;
        }
        else if(e<list[mid]) {
            high = mid-1;
        }
        else {
            low = mid+1;
        }
    }
    //在块中顺序查找
    if(low<idxcount)
    {
        for(i=idx[low].index;i<idx[low].index+idx[low].size-1 && i<count;i++)
        {
            if(list[i] == key) return i;
        }
    }
    return -1;
}


//***********************************二叉树查找 时间复杂度O（lgN） 最差为O(N)
//树节点
template<class T>
class TNode
{
    T key;//节点关键字
    TNode<T> *lc,*rc;//左右节点
};

//head 为树根 p为当前父节点 q为当前节点 key 为所要查找
template<class T>
bool TreeSearch(TNode<T> *head,TNode<T> **p,TNode<T> **q,T key)
{
    *q=head;
    while(*q) //从根节点找
    {
        if(key>(*q)->key) //key大则找右树
        {
            *p = *q;
            *q = (*q)->rc;
        }
        else if(key<(*q)->key)//key小则找左树
        {
            *p = *q;
            *q = (*q)->lc;
        }
        else
            return true;
    }
    return false;
}

//插入节点
template<class T>
bool InsertNode(TNode<T> *head,T key)
{
    TNode<T> *p = *head,*q,*s;
    if(!TreeSearch(*head,&p,&q,key))
    {
        s = new TNode<T>;
        s->key = key;
        s->lc = NULL;
        s->rc = NULL;
        if(!p) //如果是空树
            head = s;
        else
        {
            if(key>p->key) //
                p->rc = s;
            else
                p->lc = s;
        }
        return true;
    }
    return false;
}



//*********************************************************************散列值Hash查找
/*
其实常用的做哈希的手法有“五种”：
第一种：”直接定址法“。
很容易理解，key=Value+C； 这个“C"是常量。Value+C其实就是一个简单的哈希函数。
第二种：“除法取余法”。
很容易理解， key=value%C;解释同上。
第三种：“数字分析法”。
这种蛮有意思，比如有一组value1=112233，value2=112633，value3=119033，
针对这样的数我们分析数中间两个数比较波动，其他数不变。那么我们取key的值就可以是
key1=22,key2=26,key3=90。 
第四种：“平方取中法”。此处忽略，见名识意。
第五种：“折叠法”。
这种蛮有意思,比如value=135790，要求key是2位数的散列值。那么我们将value变为13+57+90=160，
然后去掉高位“1”,此时key=60，哈哈，这就是他们的哈希关系，这样做的目的就是key与每一位value都相
关，来做到“散列地址”尽可能分散的目地。
*/
template<class T>
void InsertHash(T list[],int size,T key)
{
    int hashAddr = key%13;
    while(list[hashAddr]!=0)
    {
        hashAddr = (++hashAddr)%size;
    }
    list[hashAddr]=key;
}

template<class T>
int HashSearch(T list[], int size,int key)
{
    int hashAddr = key % 13;
    while(list[hashAddr] !=0 && hash[Addr] != key)
    {
        hashAddr = (++hashAddr) % size;
    }

    if(list[hashAddr]==0) return -1;
    return hashAddr;
}


}}


#endif 

