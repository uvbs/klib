#ifndef _klib_antivr_detect_h_
#define _klib_antivr_detect_h_

#include <macro.h>
#include <vector>
#include <string>

#include "../../../core/singleton.h"
using namespace klib::core;

enum e_antvir_type
{
    e_antvir_invalid,
    e_antvir_360_sd = 1,
    e_antvir_baidu,
    e_antvir_qq_pc_mgr,
    e_antvir_king,
    e_antvir_other,
};

typedef std::vector<std::string>  proc_name_list_type;
struct antvir_process_info
{
    antvir_process_info()
    {}

    DEFINE_ACCESS_FUN(e_antvir_type, e_antvir_);
    DEFINE_ACCESS_FUN_REF(proc_name_list_type, proc_name_lst_);

    bool is_match(const char* proc_name)
    {
        for (auto itr = this->proc_name_lst_.begin();
             itr != this->proc_name_lst_.end();
             ++ itr)
        {
            if (strnicmp(proc_name, itr->c_str(), itr->size()) == 0) {
                return true;
            }
        }

        return false;
    }

protected:
    e_antvir_type  e_antvir_;

    proc_name_list_type  proc_name_lst_;
};

class antvir_detector : public singleton<antvir_detector>
{
public:
    bool init();
    e_antvir_type get_detected_antvir();

protected:
    std::vector<antvir_process_info>  antvir_proc_list_;
};




#endif  // _klib_antivr_detect_h_