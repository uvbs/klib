/*******************************************************************************
*
* FileName : test_for_graph.cpp
* Comment  : test code for graph.h
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-10-2 13:20
*
*******************************************************************************/

#include <iostream>
#include "graph.h"

int main()
{
	Graph_matrix<int,int> graph(20,10000,-1);
	graph.insert(0);
	graph.insert(1);
	graph.insert(0,0,1);
	graph.dfs();
	std::cout<<'\n';
	graph.bfs();
	std::cout<<'\n';
	graph.erase(1);
	graph.dfs();
	std::cout<<'\n';
	graph.bfs();
	std::cout<<'\n';
	return 0;
}