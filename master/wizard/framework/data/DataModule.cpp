#include "stdafx.h"
#include "DataModule.h"

#include <db/CppSQLite3.h>


CDataModule::CDataModule(void)
{
}


CDataModule::~CDataModule(void)
{
}

void CDataModule::on_pre_run()
{
    klib::db::CppSQLite3DB db;

}

BOOL CDataModule::on_run()
{
    return TRUE;
}

void CDataModule::on_post_run()
{
}

void CDataModule::on_pre_stop()
{
}

void CDataModule::on_stop()
{
}

void CDataModule::on_post_stop()
{
}