#include "res_mgr.h"



void spicify_res::set_mgr(spicify_res_mgr_i* mgr)
{  
    mgr_ = mgr; 
}

void spicify_res::release() 
{ 
    mgr_->release(this);
}

////////////////////////////////////////////////////////////////
////////

void res_mgr_imp::add_res(spicify_res_mgr_i* res, void* p)
{
#ifdef _DEBUG
    res_alloc_item* item = allic_item_reuser_.create_object();
    item->op_   = e_op_add;
    item->res_p = p;
    item->spic_res_ = res;

    // push to list;
    queue_.push(item);
#else
    
#endif
}

void res_mgr_imp::del_res(spicify_res_mgr_i* res, void* p)
{
#ifdef _DEBUG
    res_alloc_item* item = allic_item_reuser_.create_object();
    item->op_   = e_op_del;
    item->res_p = p;
    item->spic_res_ = res;

    // push to list;
    queue_.push(item);
#else
    
#endif
}


bool res_mgr_imp::reg(spicify_res_mgr_i* res)
{
    res_type_lst_.push_back(res);
    return true;
}


void res_mgr_imp::worker()
{
    do
    {
        res_alloc_item* item = nullptr;
        while (queue_.pop(item))
        {
            if (e_op_add == item->op_)
            {

            }
            else if (e_op_del == item->op_)
            {
                item->spic_res_->release(item->res_p);
            }
        } 


    } while (true);
    
}