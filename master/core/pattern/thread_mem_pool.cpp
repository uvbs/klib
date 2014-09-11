#include "thread_mem_pool.h"

tm_pool::tm_pool(size_t size, size_t bknum) :
    next(NULL), mbk(NULL),
    bksize(size), onebknum(bknum)
{
   thid = GetCurrentThreadId();
}
 
tm_pool::~tm_pool()
{
        destroy();
}
 
void tm_pool::destroy()
{
    for(tm_memblock *p = mbk; p; )
    {
        tm_memblock *q = p->next;
        free((char *)p);
        p = q;
    }
    mbk  = NULL;
    next = NULL;
}
 
void *tm_pool::newobj()
{
    if(! next)
    {
        gcontrol.lock();
        if(gcontrol.gfree)
        {
            next = gcontrol.gfree;
            gcontrol.gfree = NULL;
        }
        gcontrol.unlock();
    }
    if(! next)
    {
        expand();
    }
    tm_bufunit *head = next;
    next = head->next;
//     return (void *)head;
    return (void *)head->data;
}

void tm_pool::delobj(void *pbuf)
{
//     tm_bufunit *head = (tm_bufunit*)(pbuf);
    tm_bufunit *head = (tm_bufunit *)((char *)pbuf-offsetof(tm_bufunit, data));
    tm_pool *pool = head->pool;
    if(pool->thid == GetCurrentThreadId())
    {
        head->next = pool->next;
        pool->next = head;
    }
    else
    {
        pool->gcontrol.free(head);
    }
}
 
void tm_pool::expand()
{
    size_t unitsize = offsetof(tm_bufunit, data) + bksize;
    size_t size = (unitsize * onebknum + sizeof(tm_memblock));
    tm_memblock *pbk = (tm_memblock *)malloc(size);
    pbk->next = mbk;
    mbk = pbk;
    tm_bufunit *p = (tm_bufunit*)((char *)pbk+sizeof(tm_memblock));
    p->pool = this;
    next = p;
    for(size_t i=0; i<onebknum-1; ++i)
    {
            p->next = (tm_bufunit *)((char *)p+unitsize);
            p = p->next;
            p->pool = this;
    }
    p->next = NULL;
}

// malloc 申请的打上标记
static void *tm_malloc(size_t size, size_t *osize=NULL)
{
    tm_bufunit *p = (tm_bufunit *)malloc(sizeof(tm_bufunit)-offsetof(tm_bufunit, data)+size);
    if(p)
    {
        p->pool = NULL;
        if(osize) *osize = size;
        return p->data;
    }
    return  NULL;
}