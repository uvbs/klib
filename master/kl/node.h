#ifndef _kl_node_h_
#define _kl_node_h_


#include <vector>
#include <list>

#define MAX_CHILDREN 3

enum node_type
{
    type_statement,         // statement
    type_expression,        // expression
};

enum node_subtype
{
    function_define,
    variable_define,
    param_define,
    add_op,
    sub_op,
    mul_op,
    div_op,

    assign_stmt,
    if_stmt,
    ifelse_stmt,
    else_stmt,
    while_stmt,
    return_stmt,

    function_call,
    const_id,
    variable_id,

    other,
};


/* Óï·¨Ê÷½áµã */
class sytax_node
{
public:
    sytax_node() {}
    sytax_node(node_type type) : type_(type) {}

public:
    node_type type_;                        // node type
    node_subtype subtype_;                  // node subtype

    std::vector<sytax_node*> childs_[MAX_CHILDREN];
    std::list<sytax_node*>   siblings_;
};




#endif