#include <set>
#include <cstdlib>
#include <iostream>
#include "evolution.h"

using namespace std;

UniformEvolutionModel::UniformEvolutionModel(int m0) {
	UniformEvolutionModel::m0 = m0;
}


UniformEvolutionModel::~UniformEvolutionModel() {
	//
}

void UniformEvolutionModel::evolution_step(MultiplexNetwork& mnet, network_id net)  {
	std::set<global_vertex_id> new_vertexes;
	std::set<global_edge_id> new_edges;
	evolution_step(mnet, net, new_vertexes, new_edges);
}


void UniformEvolutionModel::evolution_step(MultiplexNetwork& mnet, network_id net, std::set<global_vertex_id>& new_vertexes, std::set<global_edge_id>& new_edges) {
	// Randomly pick two vertexes (uniform probability) and connect them
	vertex_id v1 = rand.getElement(mnet.getNetwork(net).getVertexes());
	vertex_id v2 = rand.getElement(mnet.getNetwork(net).getVertexes());
	if (!mnet.getNetwork(net).containsEdge(v1,v2))
		mnet.getNetwork(net).addEdge(v1,v2);
}


void UniformEvolutionModel::init_step(MultiplexNetwork& mnet, network_id net) {
	std::set<entity_id> ids = rand.getKElements(mnet.getGlobalIdentities(), m0);
	for (entity_id id: ids) {
		vertex_id v = mnet.getNetwork(net).addVertex(mnet.getGlobalName(id));
		mnet.mapIdentity(id,v,net);
	}
}





