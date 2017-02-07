#include <set>
#include <cstdlib>
#include <iostream>
#include "generation.h"

using namespace std;

namespace mlnet {

UniformEvolutionModel::UniformEvolutionModel(uint m0) {
	UniformEvolutionModel::m0 = m0;
}


UniformEvolutionModel::~UniformEvolutionModel() {
	//
}

void UniformEvolutionModel::evolution_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer)  {
	std::set<NodeSharedPtr> new_nodes;
	std::set<EdgeSharedPtr> new_edges;
	evolution_step(mnet, layer, new_nodes, new_edges);
}


void UniformEvolutionModel::evolution_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer, std::set<NodeSharedPtr>& new_nodes, std::set<EdgeSharedPtr>& new_edges) {
	// Randomly pick two vertexes (uniform probability) and connect them
	NodeSharedPtr v1 = mnet->get_nodes(layer)->get_at_random();
	NodeSharedPtr v2 = mnet->get_nodes(layer)->get_at_random(); // this allows self-edges
	if (!mnet->get_edge(v1,v2))
		mnet->add_edge(v1,v2);
}

void UniformEvolutionModel::init_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer) {
	if (mnet->get_actors()->size()<m0)
		throw WrongParameterException("not enough actors available to initialize the layer (less than m0)");
	std::set<ActorSharedPtr> actors;
	while (actors.size()<m0)
		actors.insert(mnet->get_actors()->get_at_random());
	// we assume that the layer is empty - otherwise, some duplicate actors might accur
	for (ActorSharedPtr actor: actors) {
		mnet->add_node(actor, layer);
	}
}

}



