#pragma once

#include "object_reuser.h"
#include "multi_buffer_queue.h"
#include "../kthread/atomic.h"
#include "../util/is_derive.h"

using namespace klib::kthread;
using namespace klib::pattern;

#include <list>
#include <vector>
// ╤сап

class res_mgr_i;
class spicify_res_mgr_i;

template <typename T>
class spicify_res_mgr;


class spicify_res
{
    friend spicify_res_mgr_i;

protected:
    virtual ~spicify_res() {}

public:
    void release() ;
    void set_mgr(spicify_res_mgr_i* mgr) ;

protected:
    spicify_res_mgr_i* mgr_;
};

class spicify_res_mgr_i
{
public:
    virtual void set_desc(const std::string& desc)  { desc_ = desc; }
    virtual const std::string& get_desc()           {  return desc_; }

    virtual void  release(void* p) { curr_count_.dec(); }
    
    size_t  get_alloc_count();
    size_t  get_curr_count();

protected:
    std::string                 desc_;
    
    AtomicInt32                 alloc_count_;
    AtomicInt32                 curr_count_;
};


// to define to be singleton
template <typename TRes>
class spicify_res_mgr : public spicify_res_mgr_i
{
public:
    TRes* create()
    {
        static_assert(TIsDerived<TRes, spicify_res>::Result, "error derived");

        TRes* p = new TRes;
        p->set_mgr(this);
        
        curr_count_.inc();
        alloc_count_.inc();

        return p;
    }

    void  release(TRes* p)
    {
        delete p;
        curr_count_.dec();
    }

protected:
    // stats relative

};


////////////////////////////////////////////////
///  place list;
class res_mgr_i
{
public:
    virtual bool reg(spicify_res_mgr_i* res) = 0;

    virtual void add_res(spicify_res_mgr_i* res, void* p) = 0;
    virtual void del_res(spicify_res_mgr_i* res, void* p) = 0;

};

class res_mgr_imp : public res_mgr_i
{
public:
    enum item_op
    {
        e_op_add,
        e_op_del,
    };

    struct res_alloc_item
    {
        item_op         op_;
        spicify_res_mgr_i*  spic_res_;
        void*           res_p;
    };
        
    virtual  bool reg(spicify_res_mgr_i* res);
    virtual void add_res(spicify_res_mgr_i* res, void* p);
    virtual void del_res(spicify_res_mgr_i* res, void* p);

protected:
    void worker();

protected:
    object_reuser<res_alloc_item> allic_item_reuser_;
    multi_buffer_queue<1000000, 5, res_alloc_item*> queue_;
    
    std::vector<spicify_res_mgr_i*>  res_type_lst_;
    // start a thread to deal the queue info

};