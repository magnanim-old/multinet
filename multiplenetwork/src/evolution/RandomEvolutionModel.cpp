#include <set>
#include <cstdlib>
#include <iostream>
#include "evolution.h"

using namespace std;

RandomEvolutionModel::RandomEvolutionModel(int m0) {
	RandomEvolutionModel::m0 = m0;
}


RandomEvolutionModel::~RandomEvolutionModel() {
	//
}


void RandomEvolutionModel::evolution_step(MultiplexNetwork& mnet, network_id net) {
	// Randomly pick two vertexes (uniform probability) and connect them
	vertex_id v1 = rand.getElement(mnet.getNetwork(net).getVertexes());
	vertex_id v2 = rand.getElement(mnet.getNetwork(net).getVertexes());
	if (!mnet.getNetwork(net).containsEdge(v1,v2))
		mnet.getNetwork(net).addEdge(v1,v2);
}


void RandomEvolutionModel::init_step(MultiplexNetwork& mnet, network_id net) {

	std::set<global_identity> ids = rand.getKElements(mnet.getGlobalIdentities(), m0);
	for (global_identity id: ids) {
		vertex_id v = mnet.getNetwork(net).addVertex(mnet.getGlobalName(id));
		mnet.mapIdentity(id,v,net);
	}
}





