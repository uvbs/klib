/*******************************************************************************
*
* FileName : graph_list.h
* Comment  : graph implemented in adjacency list
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-- :
*
*******************************************************************************/

#ifndef _GRAPH_LIST_H_INCLUDED
#define _GRAPH_LIST_H_INCLUDED

#include<vector>
#include "../../slist/slist/slist.h"

using std::vector;

template<typename T_vertex, typename T_edge>
class Graph_list {
public:
	Graph_list(int max_size);
	~Graph_list();

private:
	struct Edge_node {
		int index;
		T_edge* edge_info;
		Edge_node* next;
	};
	struct Vert_node {
		T_vertex* vert_info;
		Slist<T_edge>* edge;
	};
	int hb_num_vert;
	int hb_num_edge;
	vector<Vert_node> vertex;
};

template<typename T_vertex, typename T_edge>
Graph_list<T_vertex,T_edge>::Graph_list(int max_size)
{
	vertex.resize(max_size);
}

template<typename T_vertex, typename T_edge>
Graph_list<T_vertex,T_edge>::~Graph_list()
{
}

#endif