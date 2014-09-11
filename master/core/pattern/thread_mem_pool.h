struct tm_bufunit
{
    tm_pool *pool;                        //pool指针
    union
    {
        tm_bufunit *next;   //下一个块指针
        char data[4];           //数据区域
    };
};
 
struct tm_gcontrol
{
    tm_bufunit *gfree;
    CRITICAL_SECTION gcs;

    tm_gcontrol() : gfree(NULL) { InitializeCriticalSection(&gcs); }
    ~tm_gcontrol()        { DeleteCriticalSection(&gcs); }
    Inline void lock()     { EnterCriticalSection(&gcs); }
    Inline void unlock() { LeaveCriticalSection(&gcs); }
    void free(tm_bufunit *buf)
    {
        lock();
        buf->next = gfree;
        gfree = buf;
        unlock();
    }
};
 
struct tm_memblock
{
   tm_memblock *next;
};
 
class tm_pool
{
private:
    size_t bksize;                   //一个分配块大小
    size_t onebknum;            //一次分配多少个bksize
    DWORD thid;                          //线程id
    tm_bufunit *next;           //pool中自由块链
    tm_memblock *mbk;              //trunk表
    tm_gcontrol gcontrol;     //全局free表
   
    friend tm_poolset;
private:
    void expand();

public:
    tm_pool(size_t size, size_t bknum);
    ~tm_pool();

    void destroy();
    void *newobj();
    static void delobj(void *pbuf);
};
 
class tm_poolset
{
public:
    tm_poolset();
    virtual ~tm_poolset();

    //添加分配池
    bool addpool(size_t size, size_t allocnum);
    void *newobj(size_t size, size_t *osize=NULL);
    void delobj(void *pbuf, size_t size);
    void destroy();

    tm_pool *findpool(size_t size)
	{
        TMPOOLS::iterator it = tmpools.lower_bound(size);
        if(it != tmpools.end())
                return it->second;
        return NULL;
    }
protected:
    typedef std::map<size_t, tm_pool *> TMPOOLS;
    TMPOOLS tmpools;
};
 
//公开的数据及函数
extern DWORD tm_tlsindex;  //tls索引
 
//app初始化，分配index
void tm_init();
void tm_free();
 
//关联到该线程
void tm_attach();
void tm_detach();
 
tm_poolset *tm_getpoolset();
//添加trunk
bool tm_addtrunk(size_t size, size_t allocnum);
//tls相关分配
void *tm_new(size_t size, size_t *osize=NULL);
//tls相关释放
void tm_del(void *buf, size_t size);
 
