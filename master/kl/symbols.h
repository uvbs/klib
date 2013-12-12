
#ifndef _kl_symbols_h_
#define _kl_symbols_h_

#include "variable.h"

#include <map>
#include <string>

typedef variable symbol_info;

typedef std::map<std::string, symbol_info> symbols_info_table;

//----------------------------------------------------------------------
// ·ûºÅ¹ÜÀíÆ÷
class symbols_mgr 
{
public:
    bool add_symbol(const std::string& name, symbol_info& info);
    bool set_symbol(const std::string& name, symbol_info& info);
    bool get_symbol(const std::string& name, symbol_info& info);

protected:
    symbols_info_table symbol_table_;

};


#endif //   _kl_symbols_h_