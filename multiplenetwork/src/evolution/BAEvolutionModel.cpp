/*
 * BAEvolutionModel.cpp
 *
 *  Created on: 13/ago/2013
 *      Author: matteo.magnani
 */

#include "evolution.h"
#include <set>
#include <iostream>

using namespace std;

void ba_choice(Network& graph, int m, set<vertex_id>& chosen);

BAEvolutionModel::BAEvolutionModel(int m) {
	num_of_neighbors = m;
}

BAEvolutionModel::~BAEvolutionModel() {
}


void BAEvolutionModel::init_step(MultilayerNetwork& mnet, network_id net) {
	std::cout << "BA init" << std::endl;
}

void BAEvolutionModel::evolution_step(MultilayerNetwork& mnet, network_id net) {

	Random rand;

	std::set<intralayer_edge_id> universe;
	mnet.getVertexes(universe);
	std::set<intralayer_edge_id> choice;
	rand.getKElements(universe,choice,1);
	intralayer_edge_id new_global_vertex = *choice.begin();

	std::cout << "BA add " << new_global_vertex << std::endl;

    // Randomly select m nodes with probability proportional to their degree

	/*
	set<vertex_id> target_nodes;
	ba_choice(mnet->getNetwork(net),num_of_neighbors,target_nodes);

	// Add a new node to this network, taken from the Universe
	vertex_id new_vertex = mnet.getNetwork(net)->addVertex();
	mnet.map(global_vertex,new_vertex,net);
	//mnet->

	for (set<vertex_id>::iterator node=target_nodes.begin(); node!=target_nodes.end(); ++node) {
		mnet.getNetwork(layer)->addEdge(new_vertex,*node);
	}
	*/
}

void ba_choice(Network& net, int m, std::set<long>& selected_nodes) {
	/*
	long num_nodes = net.getNumVertexes();
	long num_edges = net.getNumEdges();


	if (m>=num_nodes) {
		for (int i=0; i<num_nodes; i++) {
			selected_nodes.insert(i);
		}
	}
	else while (selected_nodes.size()<m) {
		igraph_integer_t nodes[2];
		igraph_edge(graph,rand()%num_edges,&nodes[0],&nodes[1]);
		//long fromGlobalId = mnet->getGlobalId(layer,from);
		//long toGlobalId = mnet->getGlobalId(layer,to);
		long node = nodes[rand()%2];
		selected_nodes.insert(node);
	}
	return selected_nodes;*/
}
