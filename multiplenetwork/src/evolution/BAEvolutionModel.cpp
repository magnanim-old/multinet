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
			mnet.map(name,name,mnet.getNetworkName(net));
		}
	}
	for (std::string name1: names) {
		for (std::string name2: names) {
			if (name1<name2)
				mnet.getNetwork(net).addEdge(name1,name2);
		}
	}
}

void BAEvolutionModel::evolution_step(MultiplexNetwork& mnet, network_id net)  {
	std::set<global_vertex_id> new_vertexes;
	std::set<global_edge_id> new_edges;
	evolution_step(mnet, net, new_vertexes, new_edges);
}


void BAEvolutionModel::evolution_step(MultiplexNetwork& mnet, network_id net, std::set<global_vertex_id>& new_vertexes, std::set<global_edge_id>& new_edges) {

	std::string new_vertex = rand.getElement(mnet.getGlobalNames());

	if (mnet.getNetwork(net).containsVertex(new_vertex))
		return;

	//std::cout << "Inserting vertex " + new_vertex;
	vertex_id vid = mnet.getNetwork(net).addVertex(new_vertex);
	//std::cout << " " << mnet.getNetwork(net).getNumVertexes() << std::endl;
	mnet.map(new_vertex,new_vertex,mnet.getNetworkName(net));
	global_vertex_id new_vertex_id(vid,net);
	new_vertexes.insert(new_vertex_id);

    // Randomly select m nodes with probability proportional to their degree and connect them to new_vertex
	std::set<edge_id> all_edges = mnet.getNetwork(net).getEdges();
	std::set<edge_id> edges = rand.getKElements(all_edges, m);


	for (edge_id edge: edges) {
		entity_id target_id = mnet.getGlobalIdentity(rand.test(.5)?edge.v1:edge.v2, net);
		std::string target = mnet.getGlobalName(target_id);
		if (!mnet.getNetwork(net).containsEdge(new_vertex,target)) {
			mnet.getNetwork(net).addEdge(new_vertex,target);
			global_edge_id new_edge_id(vid,mnet.getVertexId(target_id,net),net,mnet.getNetwork(net).isDirected());
			//std::cout << "I " << vid << " " << mnet.getVertexId(target_id,net) << std::endl;
			new_edges.insert(new_edge_id);
		}
	}

}
