#ifndef _kl_node_h_
#define _kl_node_h_


#include <vector>
#include <list>

#define MAX_CHILDREN 3

typedef enum
{
    type_statement,         // statement
    type_expression,        // expression
} node_type;


/* Óï·¨Ê÷½áµã */
class sytax_node
{
public:
    node_type type_;


    std::vector<sytax_node*> childs_[MAX_CHILDREN];
    std::list<sytax_node*>   siblings_;
};




#endif