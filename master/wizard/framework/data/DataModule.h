#pragma once

#include "core/singleton.h"

//----------------------------------------------------------------------
// 数据模块，主要提供数据交互相关的操作
//   如： 连接数据库，创建数据，环境初始化等
class CDataModule:
    public klib::aom::module_imp
{
public:
    CDataModule(void);
    ~CDataModule(void);


    BEGIN_INTERFACE_ENTRY
    END_INTERFACE_ENTRY

protected:
    void on_pre_run();
    BOOL on_run();
    void on_post_run();
    void on_pre_stop();
    void on_stop();
    void on_post_stop();

};

