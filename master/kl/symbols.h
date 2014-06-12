
#ifndef _kl_symbols_h_
#define _kl_symbols_h_

#include "node.h"
#include "variable.h"
#include "token.h"

#include <map>
#include <string>


// symbol type define
enum symbol_type
{
    symbol_none,
    symbol_function,    // function
    symbol_variable,    // variable
    symbol_refrence,    // refrence
};

// symbol information
struct symbol_info
{
    symbol_info() : type_(symbol_none), node_(NULL) {}

    symbol_type type_;      // symbol type
    sytax_node* node_;      // sytax node -->> to run function or execute statement
    token       token_;     // valid for refrence
};

// symbol table type
typedef std::map<std::string, symbol_info> symbols_info_table;

//----------------------------------------------------------------------
// symbol manager
class symbols_mgr 
{
public:
    bool add_symbol(const std::string& name, symbol_info& info);
    bool set_symbol(const std::string& name, symbol_info& info);
    bool get_symbol(const std::string& name, symbol_info& info);

    void clear();
protected:
    symbols_info_table symbol_table_;
};

//----------------------------------------------------------------------
// plug manager, when execute search plguin_mgr first
class plugin_mgr
{
public:
    

private:

};


// internel env
class inter_env
{
public:
    symbols_mgr* get_global_symb() { return & global_symbole_mgr_; }
    symbols_mgr* get_local_symb() { return & local_symbole_mgr_;  }
    plugin_mgr*  get_plugin_mgr() { return & plugin_mgr_;  }

protected:
    symbols_mgr global_symbole_mgr_;
    symbols_mgr local_symbole_mgr_;
    plugin_mgr  plugin_mgr_;
};

#endif //   _kl_symbols_h_