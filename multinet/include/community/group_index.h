//
//  group_index.h
//  group_index
//
//  Created by Lucas Jeub on 24/10/2012.
//
//  Implements the group_index datastructure:
//
//      nodes: vector storing the group memebership for each node
//
//      groups: vector of lists, each list stores the nodes assigned to the group
//
//      nodes_iterator: vector storing the position of each node in the list corresponding
//                      to the group it is assigned to (allows constant time moving of nodes)
//
//
//      index(group): return matlab indeces of nodes in group
//
//      move(node,group): move node to group
//
//      export_matlab(matlab_array): output group vector to matlab_a
//
//
//  Last modified by Lucas Jeub on 25/07/2014
//
// Version: 2.1
// Date: Tue 29 Nov 2016 15:29:58 EST

#ifndef GROUP_INDEX_H
#define GROUP_INDEX_H

#include <list>
#include <vector>
#include <algorithm>

namespace mlnet {

struct group_index {
	group_index();
	group_index(std::vector<int> v); //assign group index from matlab

	void move(int node, int group); //move node to group

	size_t n_nodes;
	size_t n_groups;

	std::vector<std::list<int>> groups; //the index of each node in a group is stored in a linked list
	std::vector<std::list<int>::iterator> nodes_iterator; //stores the position of the node in the list for the group it belongs to
	std::vector<int> nodes; //stores the group a node belongs to


};

}

#endif


