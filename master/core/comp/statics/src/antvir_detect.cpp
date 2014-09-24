#include "antvir_detect.h"

#include <core/process_enumer.h>
using namespace klib::core;

/*
QQPCRTP.EXE  和QQPCTray.exe   QQ电脑管家  
360safe.exe  360sd.exe  360tray.exe    360的
baidusdSvc.exe  baidusdTray.exe    百度杀毒
kxescore.exe和kxetray.exe          金山毒霸
*/

//@todo do some mixed encoding


bool antvir_detector::init ()
{
    antvir_process_info info;
    info.set_e_antvir_(e_antvir_qq_pc_mgr);
    info.get_proc_name_lst_().push_back("QQPCRTP.EXE");
    info.get_proc_name_lst_().push_back("QQPCTray.exe");
    antvir_proc_list_.push_back(info);

    info.set_e_antvir_(e_antvir_360_sd);
    info.get_proc_name_lst_().clear();
    info.get_proc_name_lst_().push_back("360safe.exe");
    info.get_proc_name_lst_().push_back("360sd.exe");
    info.get_proc_name_lst_().push_back("360tray.exe");
    antvir_proc_list_.push_back(info);

    info.set_e_antvir_(e_antvir_baidu);
    info.get_proc_name_lst_().clear();
    info.get_proc_name_lst_().push_back("baidusdSvc.exe");
    info.get_proc_name_lst_().push_back("baidusdTray.exe");
    antvir_proc_list_.push_back(info);

    info.set_e_antvir_(e_antvir_king);
    info.get_proc_name_lst_().clear();
    info.get_proc_name_lst_().push_back("kxescore.exe");
    info.get_proc_name_lst_().push_back("kxetray.exe");
    antvir_proc_list_.push_back(info);
    return true;
}

e_antvir_type antvir_detector::get_detected_antvir()
{
    process_enumer enumor;
    PROCESSENTRY32 info;
    while (enumor.get_next_process_info(info))
    {
        info.szExeFile;

        for (auto itr = this->antvir_proc_list_.begin();
             itr != this->antvir_proc_list_.end();
             ++ itr)
        {
            if (itr->is_match(info.szExeFile)) 
            {
                return itr->get_e_antvir_();
            }
        }
    }

    return e_antvir_other; 
}