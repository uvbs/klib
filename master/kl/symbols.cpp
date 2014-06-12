
#include "symbols.h"


//----------------------------------------------------------------------
// 
bool symbols_mgr::add_symbol(const std::string& name, symbol_info& info)
{
    symbols_info_table::_Pairib result;

    result = symbol_table_.insert(std::make_pair(name, info));

    return result.second;
}

bool symbols_mgr::set_symbol(const std::string& name, symbol_info& info)
{
    symbol_table_[name] = info;
    return true;
}

bool symbols_mgr::get_symbol(const std::string& name, symbol_info& info)
{
    auto itr = symbol_table_.find(name);
    if (itr != symbol_table_.end()) {
        info = itr->second;
        return true;
    }

    return false;
}

void symbols_mgr::clear()
{
    symbol_table_.clear();
}