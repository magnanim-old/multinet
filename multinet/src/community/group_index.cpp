//
//  group_index.cpp
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
//      index(group): return matlab indeces of nodes in group
//
//      move(node,group): move node to group
//
//      export_matlab(matlab_array): output group vector to matlab_array
//
//
//
//  Last modified by Lucas Jeub on 20/02/2013
//
// Version: 2.1
// Date: Tue 29 Nov 2016 15:29:58 EST

#include "community/group_index.h"
#include <iostream>

namespace mlnet {

using namespace std;


group_index::group_index():n_nodes(0), n_groups(0){}

group_index::group_index(std::vector<int> v){
	n_nodes = v.size();
	nodes.resize(n_nodes);
	nodes_iterator.resize(n_nodes);

	for(size_t i = 0; i < n_nodes; i++){
		nodes[i] = v[i];
	}

	n_groups = *max_element(nodes.begin(),nodes.end()) + 1;
	groups.resize(n_groups);

	for(size_t i = 0; i <n_nodes; i++){
		groups[nodes[i]].push_back(i);
		nodes_iterator[i]= --groups[nodes[i]].end();
	}

}

//moves node to specified group
void group_index::move(int node, int group){
    //move node by splicing into list for new group
	groups[group].splice(groups[group].end(), groups[nodes[node]],nodes_iterator[node]);
    //update its group asignment
	nodes[node]=group;
}


vector<int> group_index::toVector() {
	
	vector<int> v (n_nodes);
    vector<bool> track_move(n_nodes, true);
    size_t g_n = 0;

	list<int>::iterator it;

	for(size_t i = 0; i < n_nodes; i++){
		if(track_move[i]){
			for(it=groups[nodes[i]].begin(); it != groups[nodes[i]].end();it++){
				v[*it] = g_n;
                track_move[*it] = false;
			}
            g_n++;
		}
	}	
	return v;
}


}

