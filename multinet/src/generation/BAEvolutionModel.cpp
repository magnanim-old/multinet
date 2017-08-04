/*
 * BAEvolutionModel.cpp
 *
 *  Created on: 13/ago/2013
 *      Author: matteo.magnani
 */

#include "generation.h"
#include <set>
#include <iostream>

using namespace std;

namespace mlnet {

BAEvolutionModel::BAEvolutionModel(size_t m0, size_t m) {
	BAEvolutionModel::m0 = m0;
	BAEvolutionModel::m = m;
}

BAEvolutionModel::~BAEvolutionModel() {
}

void BAEvolutionModel::init_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer) {
	if (mnet->get_actors()->size()<m0)
		throw WrongParameterException("not enough actors available to initialize the layer (less than m0)");
	std::set<ActorSharedPtr> actors;
	while (actors.size()<m0)
		actors.insert(mnet->get_actors()->get_at_random());
	// we assume that the layer is empty - otherwise, some duplicate actors might be selected
	for (ActorSharedPtr actor: actors) {
		mnet->add_node(actor, layer);
	}
	for (ActorSharedPtr actor1: actors) {
		NodeSharedPtr node1 = mnet->get_node(actor1, layer);
		for (ActorSharedPtr actor2: actors) {
			NodeSharedPtr node2 = mnet->get_node(actor2, layer);
			if (node1!=node2) {
				if (!mnet->get_edge(node1,node2)) {
					mnet->add_edge(node1,node2);
				}
			}
		}
	}
}

void BAEvolutionModel::evolution_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer)  {
	std::set<NodeSharedPtr> new_nodes;
    std::set<EdgeSharedPtr> new_edges;
    evolution_step(mnet, layer, new_nodes, new_edges);
}


void BAEvolutionModel::evolution_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer, std::set<NodeSharedPtr>& new_nodes, std::set<EdgeSharedPtr>& new_edges) {

    if (mnet->get_nodes(layer)->size()==mnet->get_actors()->size())
        return;
    
	ActorSharedPtr actor = mnet->get_actors()->get_at_random();
	while (mnet->get_node(actor,layer))
		return;

	NodeSharedPtr new_node = mnet->add_node(actor,layer);
	new_nodes.insert(new_node);

    // Randomly select m nodes with probability proportional to their degree and connect them to new_vertex
	std::set<NodeSharedPtr> nodes;
	// this operation may not be very efficient - think of an alternative implementation
	while (nodes.size()<m) {
		EdgeSharedPtr edge = mnet->get_edges()->get_at_random();
		nodes.insert(test(.5)?edge->v1:edge->v2);
	}

	for (NodeSharedPtr old_node: nodes) {
		if (!mnet->get_edge(new_node,old_node)) {
			EdgeSharedPtr new_edge = mnet->add_edge(new_node,old_node);
			new_edges.insert(new_edge);
		}
	}

}

}
