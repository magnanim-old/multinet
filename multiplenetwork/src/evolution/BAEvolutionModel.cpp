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

BAEvolutionModel::BAEvolutionModel(int m0, int m) {
	BAEvolutionModel::m0 = m0;
	BAEvolutionModel::m = m;
}

BAEvolutionModel::~BAEvolutionModel() {
}

void BAEvolutionModel::init_step(MultiplexNetwork& mnet, network_id net) {
	std::set<std::string> names = rand.getKElements(mnet.getGlobalNames(), m0);
	for (std::string name: names) {
		if (!mnet.getNetwork(net).containsVertex(name)) {
			mnet.getNetwork(net).addVertex(name);
			mnet.mapIdentity(name,name,mnet.getNetworkName(net));
		}
	}
	for (std::string name1: names) {
		for (std::string name2: names) {
			if (name1<name2)
				mnet.getNetwork(net).addEdge(name1,name2);
		}
	}
}

void BAEvolutionModel::evolution_step(MultiplexNetwork& mnet, network_id net) {

	std::string new_vertex = rand.getElement(mnet.getGlobalNames());

	if (mnet.getNetwork(net).containsVertex(new_vertex))
		return;

	//std::cout << "Inserting vertex " + new_vertex;
	mnet.getNetwork(net).addVertex(new_vertex);
	//std::cout << " " << mnet.getNetwork(net).getNumVertexes() << std::endl;
	mnet.mapIdentity(new_vertex,new_vertex,mnet.getNetworkName(net));

    // Randomly select m nodes with probability proportional to their degree and connect them to new_vertex
	std::set<edge_id> all_edges = mnet.getNetwork(net).getEdges();
	std::set<edge_id> edges = rand.getKElements(all_edges, m);


	for (edge_id edge: edges) {
		global_identity target_id = mnet.getGlobalIdentity(rand.test(.5)?edge.v1:edge.v2, net);
		std::string target = mnet.getGlobalName(target_id);
		if (!mnet.getNetwork(net).containsEdge(new_vertex,target)) {
			mnet.getNetwork(net).addEdge(new_vertex,target);
		}
	}

}
