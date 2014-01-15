

//type and macro  
#include <default.h>  

//debug 信息  
const   bool    DebugQMem=1;  

struct  QMemDebugInfo{  
    uint32_t        mask;  
    uint32_t        size;  
    byte            guard[128-8];  
};  


template <bool flag, typename T, typename U>  
struct TypeSelectT  
{  
private:  
    template<bool>  
    struct In   
    { typedef T Result; };  

    template<>  
    struct In<false>  
    { typedef U Result; };  

public:  
    typedef typename In<flag>::Result Result;  
};  

//计算ln(x)/ln(y)，向上转换为整形  
template <uint64_t x,uint64_t y>  
struct logT{  
protected:  
    template <uint64_t x1>  
    struct imp{  
        enum {  
            next=(x1+y-1)/y,  
        };  
        enum {Result=1+imp<next>::Result};  
    };  

    template <>struct imp<1>{  
        enum {Result=0};  
    };  

public:  
    enum {Result=imp<x>::Result};  
};  

//x^y  
template <uint64_t x,uint64_t y>  
struct powerT{  
protected:  
    template <uint64_t y1>  
    struct imp{  
        enum{  
            next=y1-1  
        };  
        enum{  
            Result=x*imp<next>::Result  
        };  
    };  
    template<> struct imp<0>{  
        enum{  
            Result=1  
        };  
    };  

public:  
    enum{  
        Result=imp<y>::Result  
    };  
};  


//缓存表,  
template <uint64_t index>  
struct cacheCount{  

    //table<>未实现0,1,2,3,所以不允许<16b的对象编译通过，避免空间太小不能容纳调试信息。  

    template <uint64_t>   struct  table;  
    template <>   struct table<4>{ enum{    Result= 4096    };};    //16b -> 64k  
    template <>   struct table<5>{ enum{    Result= 4096    };};    //32b -> 128k  
    template <>   struct table<6>{ enum{    Result= 4096    };};    //64b -> 256k  
    template <>   struct table<7>{ enum{    Result= 4096    };};    //128b-> 512k  
    template <>   struct table<8>{ enum{    Result= 2048    };};    //256b-> 512k  
    template <>   struct table<9>{ enum{    Result= 2048    };};    //512b-> 1m  
    template <>   struct table<10>{ enum{   Result= 2048    };};    //1k  -> 2m  
    template <>   struct table<11>{ enum{   Result= 2048    };};    //2k  -> 4m  
    template <>   struct table<12>{ enum{   Result= 1024    };};    //4k  -> 4m  
    template <>   struct table<13>{ enum{   Result= 1024    };};    //8k  -> 8m  
    template <>   struct table<14>{ enum{   Result= 1024    };};    //16k -> 16m  
    //更多的使用时添加  
    template <>   struct table<20>{ enum{   Result= 4       };};    //1m -> 4m  
    struct  minCache{enum   {Result=2};};  
    enum    {Result=TypeSelectT<index<20,table<logT<index,2>::Result>,minCache>::Result::Result };  
};  





//链表，用于可用空间回收  
struct  QMemLink{  
    QMemLink*   next;  
};  

class   QMemTest;  

byte*   QAlloc(size_t s){  
    return  new byte[s];  
}  

template <size_t size>   
class QFixAllocator{  

    friend  class   QMemTest;  
    uint64_t    usageBytes;  
    uint64_t    allocBytes;  
    uint64_t    poolBytes;  
    QMemLink*   memPool;  
    enum    {sizeFix=powerT<2,logT<size,2>::Result>::Result};  
    enum    {caches=cacheCount<sizeFix>::Result};  
protected:  
    QFixAllocator(){  
        usageBytes=0;  
        allocBytes=0;  
        poolBytes=0;  
        memPool=NULL;  
    }  

public:  
    void* alloc(size_t s){  
        assert(s<=sizeFix);//避免new[]call到错误的allocator,host class应该使用单独版本的new[]()，而不是默认调用new()  

        QMemLink*   r;  
        if(memPool){  
            r=memPool;  
            memPool=memPool->next;  
            poolBytes-=sizeFix;  
            usageBytes+=s;  
            return  r;  
        }  

        byte*   p=QAlloc(sizeFix*caches);  

        memPool=(QMemLink*)p;  
        for(int i=1;i<caches;++i){  
            r=(QMemLink*)p;  
            p+=sizeFix;  
            r->next=(QMemLink*)p;;  
        }  
        r->next=NULL;  
        poolBytes+=(caches-1)*sizeFix;  
        usageBytes+=s;  

        allocBytes+=caches*sizeFix;  
        return  p;  

    }  

    void free(void*p,size_t s){  
        QMemLink*   l=(QMemLink*)p;  
        l->next=memPool;  
        memPool=l;  
        usageBytes-=s;  
        poolBytes+=size;  
    }  
    void    status(uint64_t &u,uint64_t &a,uint64_t& p)const{  
        u=usageBytes;  
        a=allocBytes;  
        p=poolBytes;  
    }  

    typedef     QFixAllocator me;  

    static me Instance;  

};  



typedef QFixAllocator<powerT<2,4>::Result>  QFixAllocator_4;  
typedef QFixAllocator<powerT<2,5>::Result>  QFixAllocator_5;  
typedef QFixAllocator<powerT<2,6>::Result>  QFixAllocator_6;  
typedef QFixAllocator<powerT<2,7>::Result>  QFixAllocator_7;  
typedef QFixAllocator<powerT<2,8>::Result>  QFixAllocator_8;  
typedef QFixAllocator<powerT<2,9>::Result>  QFixAllocator_9;  
typedef QFixAllocator<powerT<2,10>::Result> QFixAllocator_10;  
typedef QFixAllocator<powerT<2,11>::Result> QFixAllocator_11;  
typedef QFixAllocator<powerT<2,12>::Result> QFixAllocator_12;  
typedef QFixAllocator<powerT<2,13>::Result> QFixAllocator_13;  
typedef QFixAllocator<powerT<2,14>::Result> QFixAllocator_14;  
typedef QFixAllocator<powerT<2,15>::Result> QFixAllocator_15;  
typedef QFixAllocator<powerT<2,16>::Result> QFixAllocator_16;  
typedef QFixAllocator<powerT<2,17>::Result> QFixAllocator_17;  
typedef QFixAllocator<powerT<2,18>::Result> QFixAllocator_18;  
typedef QFixAllocator<powerT<2,19>::Result> QFixAllocator_19;  
typedef QFixAllocator<powerT<2,20>::Result> QFixAllocator_20;  
typedef QFixAllocator<powerT<2,21>::Result> QFixAllocator_21;  


QFixAllocator_10    QFixAllocator_10::Instance;  
QFixAllocator_11    QFixAllocator_11::Instance;  
QFixAllocator_12    QFixAllocator_12::Instance;  
QFixAllocator_13    QFixAllocator_13::Instance;  
QFixAllocator_14    QFixAllocator_14::Instance;  
QFixAllocator_15    QFixAllocator_15::Instance;  
QFixAllocator_16    QFixAllocator_16::Instance;  
QFixAllocator_17    QFixAllocator_17::Instance;  
QFixAllocator_18    QFixAllocator_18::Instance;  
QFixAllocator_19    QFixAllocator_19::Instance;  
QFixAllocator_20    QFixAllocator_20::Instance;  
QFixAllocator_21    QFixAllocator_21::Instance;  





template    <class Host>  
class   QMemImpl{  
public:  

    void*   operator    new(size_t size){  
        return  QFixAllocator<powerT<2,logT<sizeof(Host),2>::Result>::Result>::Instance.alloc(size);  
    }  

    void    operator    delete(void*p){  
        return  QFixAllocator<powerT<2,logT<sizeof(Host),2>::Result>::Result>::Instance.free(p,sizeof(Host));  
    }  

    void*   operator    new[](size_t size){  

        size+=8;    //x64 8字节对齐  

        ULONG   r,f;  
        _BitScanReverse(&r,size);  
        _BitScanReverse(&f,size);  

        if(r!=f)    //对齐  
            ++r;  

        switch(r){  
        case    19://这里只处理了1m字节的分配  
            {  
                uint32_t *p=(uint32_t*)QFixAllocator_20::Instance.alloc(size);  
                *p=19;//set mask  
                ++p;  
                *p=size;  
                return  ++p;  
            }  

        default:  
            DebugBreak();  
        }  
        throw   "bad alloc";  
    }  

    void    operator    delete[](void*p){  
        uint32_t*   p32=(uint32_t*)p;  
        p32-=2;  
        switch(*p32){  
        case    19://这里只处理了1m字节的释放  
            QFixAllocator_20::Instance.free(p32,*(p32+1));  
            return;  
        default:  
            DebugBreak();  
        }  

    }  
};  

template    <class Host>  
class   QMemDebugImpl{  
public:  
    void*   operator    new(size_t size){  

        QMemDebugInfo*  p=(QMemDebugInfo*)QFixAllocator<powerT<2,logT<sizeof(Host)+sizeof(QMemDebugInfo),2>::Result>::Result>::Instance.alloc(size);  
        memset(p->guard,0,sizeof(p->guard));  
        p->mask=logT<sizeof(Host)+sizeof(QMemDebugInfo),2>::Result;  
        p->mask|=0x12345600;  
        p->size=size;  
        return  ++p;  

    }  
    void    operator    delete(void*p){  
        QMemDebugInfo*  pDebug=(QMemDebugInfo*)p;  
        --pDebug;  

        if((pDebug->mask&(~0xff))!=0x12345600){ //标志错，可能是乱用delete,new[]  
            DebugBreak();  
        }  
        if(pDebug->size!=sizeof(Host)){      //size错  
            DebugBreak();  
        }  

        //其他检查  

        return  QFixAllocator<powerT<2,logT<sizeof(Host)+sizeof(QMemDebugInfo),2>::Result>::Result>::Instance.free(pDebug,sizeof(Host));  
    }  

    void*   operator    new[](size_t size){  

        size+=sizeof(QMemDebugInfo);      
        ULONG   r,f;  
        if(!_BitScanReverse(&r,size))  
            DebugBreak();//size 0,不可能的情况  
        _BitScanReverse(&f,size);  

        if(r!=f)    //对齐  
            ++r;  
        QMemDebugInfo *p;  
        switch(r){  
        case    19:  
            p=(QMemDebugInfo*)QFixAllocator_20::Instance.alloc(size);  
            p->mask=19;  
            break;  
        case    20:  
            p=(QMemDebugInfo*)QFixAllocator_21::Instance.alloc(size);  
            p->mask=20;  
            break;  

        default:  
            DebugBreak();  
        }  
        memset(p->guard,0,sizeof(p->guard));  
        p->size=size;  
        return  ++p;  


    }  

    void    operator    delete[](void*p){  
        QMemDebugInfo *pDebug=(QMemDebugInfo*)p;  
        --pDebug;  

        if(pDebug->mask&(~0xff)){ //标志错，可能是乱用delete[],new  
            DebugBreak();  
        }  
        if(pDebug->size<sizeof(Host)){        //size错  
            DebugBreak();  
        }  

        ULONG   r,f;  
        _BitScanReverse(&r,pDebug->size);  
        _BitScanReverse(&f,pDebug->size);  
        if(r!=f)  
            ++r;  
        if(pDebug->mask!=r)  
            DebugBreak();  

        switch(pDebug->mask){  
        case    19:  
            QFixAllocator_20::Instance.free(pDebug,pDebug->size);  
            return;  
        case    20:  
            QFixAllocator_21::Instance.free(pDebug,pDebug->size);  
            return;  

        default:  
            DebugBreak();  
        }  

    }  
};  


template    <class Host>  
class   QMemPool:public TypeSelectT<DebugQMem,QMemDebugImpl<Host>,QMemImpl<Host>>::Result   
{  


}; 



/*

// from: http://blog.csdn.net/kevin_qing/article/details/7340257
class   QMemTest:public QMemPool<QMemTest>{  
char    tmp[1000];  
};  


void    showMemInfo(){  


uint64_t    u,a,p;  

QFixAllocator_11::Instance.status(u,a,p);  
printf("_11 u=%I64u a=%I64u p=%I64u \n",u,a,p);  

QFixAllocator_20::Instance.status(u,a,p);  
printf("_20 u=%I64u a=%I64u p=%I64u \n",u,a,p);  
}  

void main(){  

QMemTest*   pTest[4096];  


showMemInfo();  

for(int i=0;i<countof(pTest);++i){  
pTest[i]=new    QMemTest;  
}  
printf("new\n");  

showMemInfo();  

for(int i=0;i<countof(pTest);++i){  
delete  pTest[i];  
}  
printf("delete\n");  


showMemInfo();  
delete  new QMemTest[1024];  

printf("delete[] new[]\n");  

showMemInfo();  




}  

*/