/*******************************************************************************
*
* FileName : graph.h
* Comment  : graph implemented in adjacency matrix
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-10-2 13:20
*
*******************************************************************************/

#ifndef _GRAPH_H_INCLUDED
#define _GRAPH_H_INCLUDED

#include <iostream>
#include "../../queue/queue/queue.h"

class Error {};

template<typename T_vertex, typename T_edge>
class Graph_matrix {
public:
	Graph_matrix(int max_num, T_edge infinity, T_vertex hb_novert);
	~Graph_matrix();

	int get_num_vert() const;
	int get_num_edge() const;

	int find(T_vertex ver) const;
	virtual int next_number() const;
	int first_adj_vertex(int v) const;
	int next_adj_vertex(int v, int w) const;

	void insert(T_vertex ver);
	void insert(T_edge edge, T_vertex tail, T_vertex head);
	T_vertex erase(T_vertex ver);
	T_edge erase(T_vertex tail, T_vertex head);

	virtual void visit(T_vertex ver, 
		void (*client_visit)(T_vertex ver) = 0);

	void dfs();
	void bfs();

protected:
	void dfs_traverse(int v, bool if_visit[]);

	T_edge** adj_matrix;
	T_vertex* vertex;
	int hb_num_vert;
	int hb_num_edge;
	int hb_max_num;
	T_edge hb_infinity;
	T_vertex hb_novert;
};

template<typename T_vertex, typename T_edge>
Graph_matrix<T_vertex,T_edge>::Graph_matrix(int max_num, 
								T_edge infinity,T_vertex novert)
					: hb_num_vert(0), hb_num_edge(0), hb_max_num(max_num),
						hb_infinity(infinity), hb_novert(novert)
{
	vertex = new T_vertex[hb_max_num];
	if (0 == vertex) {
		std::cerr<<"error, out of space!\n";
		return;
	}
	for (int p=0; p!=hb_max_num; ++p) {
		vertex[p] = novert;
	}
	adj_matrix = new T_edge*[hb_max_num];
	for (int i=0; i!=hb_max_num; ++i) {
		adj_matrix[i] = new T_edge[hb_max_num];
	}
	for (int j=0; j!=hb_max_num; ++j) {
		for (int k=0; k!=hb_max_num; ++k) {
			adj_matrix[j][k] = hb_infinity;
		}
	}
}

template<typename T_vertex, typename T_edge>
Graph_matrix<T_vertex,T_edge>::~Graph_matrix()
{
	delete[] vertex;
	for (int i=0; i!=hb_max_num; ++i) {
		delete[] adj_matrix[i];
	}
	delete[] adj_matrix;
}

template<typename T_vertex, typename T_edge>
inline int Graph_matrix<T_vertex,T_edge>::get_num_vert() const
{
	return hb_num_vert;
}

template<typename T_vertex, typename T_edge>
inline int Graph_matrix<T_vertex,T_edge>::get_num_edge() const
{
	return hb_num_edge;
}

template<typename T_vertex, typename T_edge>
void Graph_matrix<T_vertex,T_edge>::visit(T_vertex ver, 
						void (*client_visit)(T_vertex ver) = 0)
{
	if (0 == client_visit)
		std::cout<<ver<<' ';
	else
		client_visit(ver);
}

template<typename T_vertex, typename T_edge>
int Graph_matrix<T_vertex,T_edge>::find(T_vertex ver) const
{
	for (int i=0; i!=hb_max_num; ++i) {
		if (vertex[i] == ver)
			return i;
	}
	return -1;	//not find
}

template<typename T_vertex, typename T_edge>
inline int Graph_matrix<T_vertex,T_edge>::next_number() const
{
	for (int i=0; i!=hb_max_num; ++i) {
		if (vertex[i] == hb_novert)
			return i;
	}
	return -1;	//out of space
}

template<typename T_vertex, typename T_edge>
int Graph_matrix<T_vertex,T_edge>::first_adj_vertex(int v) const
{
	for (int i=0; i!=hb_max_num; ++i) {
		if (adj_matrix[v][i] != hb_infinity)
			return i;
	}
	return -1;
}

template<typename T_vertex, typename T_edge>
int Graph_matrix<T_vertex,T_edge>::next_adj_vertex(int v, int w) const
{
	for (int i=w+1; i!=hb_max_num; ++i) {
		if (adj_matrix[v][i] != hb_infinity)
			return i;
	}
	return -1;
}

template<typename T_vertex, typename T_edge>
void Graph_matrix<T_vertex,T_edge>::insert(T_vertex ver)
{
	int exist = find(ver);
	if (exist != -1)	//exist
		return;
	int number = next_number();
	if (number == -1) {
		std::cerr<<"error, out of space!\n";
		throw Error();
		return;
	}
	vertex[number] = ver;
	++hb_num_vert;
}

template<typename T_vertex, typename T_edge>
void Graph_matrix<T_vertex,T_edge>::insert(T_edge edge, 
									T_vertex tail, T_vertex head)
{
	int hb_tail = find(tail);
	int hb_head = find(head);
	if (hb_tail == -1) {
		try {
			insert(tail);
		}
		catch(Error) {
			return;
		}
	}
	if (hb_head == -1) {
		try {
			insert(head);
		}
		catch(Error) {
			return;
		}
	}
	adj_matrix[tail][head] = edge;
	++hb_num_edge;
}

template<typename T_vertex, typename T_edge>
T_vertex Graph_matrix<T_vertex,T_edge>::erase(T_vertex ver)
{
	int exist = find(ver);
	if (exist == -1)
		return hb_novert;
	T_vertex temp = vertex[exist];
	vertex[exist] = hb_novert;
	--hb_num_vert;
	for (int i=0; i!= hb_max_num; ++i) {
		adj_matrix[i][exist] = hb_infinity;
		adj_matrix[exist][i] = hb_infinity;
	}
	return temp;
}

template<typename T_vertex, typename T_edge>
T_edge Graph_matrix<T_vertex,T_edge>::erase(T_vertex tail, T_vertex head)
{
	int hb_tail = find(tail);
	int hb_head = find(head);
	if (hb_tail == -1 || hb_head == -1) {
		std::cerr<<"error, the edge not exist!\n";
		return hb_infinity;
	}
	T_edge temp = adj_matrix[hb_tail][hb_head];
	adj_matrix[hb_tail][hb_head] = hb_infinity;
	return temp;
}

template<typename T_vertex, typename T_edge>
void Graph_matrix<T_vertex,T_edge>::dfs()
{
	bool* if_visit = new bool[hb_max_num];
	for (int i=0; i!=hb_max_num; ++i)
		if_visit[i] = false;
	for (int j=0; j!=hb_max_num; ++j) {
		if (!if_visit[j] && vertex[j]!=hb_novert) {
			dfs_traverse(j,if_visit);
		}
	}
}

template<typename T_vertex, typename T_edge>
void Graph_matrix<T_vertex,T_edge>::dfs_traverse(int v, bool if_visit[])
{
	visit(vertex[v]);
	if_visit[v] = true;
	for (int w = first_adj_vertex(v); w>=0; w = next_adj_vertex(v,w)) {
		if (!if_visit[w])
			dfs_traverse(w,if_visit);
	}
}

template<typename T_vertex, typename T_edge>
void Graph_matrix<T_vertex,T_edge>::bfs()
{
	bool* if_visit = new bool[hb_max_num];
	for (int i=0; i!=hb_max_num; ++i)
		if_visit[i] = false;
	Queue<int> queue;
	for (int j=0; j!=hb_max_num; ++j) {
		if (!if_visit[j] && vertex[j]!=hb_novert) {
			visit(vertex[j]);
			if_visit[j] = true;
			queue.en_queue(j);
			while (!queue.empty()) {
				int v = queue.de_queue();
				for (int w = first_adj_vertex(v); 
					w>=0; w = next_adj_vertex(v,w)) {
						if (!if_visit[w]) {
							visit(vertex[w]);
							if_visit[w] = true;
							queue.en_queue(w);
						}
				}
			}//while
		}//if(!if_visit[j])
	}//for
}

#endif