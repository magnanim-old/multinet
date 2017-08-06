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
    if (m>m0)
        throw WrongParameterException("m0 cannot be smaller than m");
}

BAEvolutionModel::~BAEvolutionModel() {
}

void BAEvolutionModel::init_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer, ActorListSharedPtr& available_actors) {
	if (available_actors->size()<m0)
		throw WrongParameterException("not enough actors available to initialize the layer (less than m0)");
    
	std::set<ActorSharedPtr> new_actors;
    // choosing the m0 initial actors
    for (size_t i=0; i<m0; i++) {
        ActorSharedPtr new_actor = available_actors->get_at_random();
		new_actors.insert(new_actor);
        available_actors->erase(new_actor);
    }
    // adding the actors to the layer
	for (ActorSharedPtr actor: new_actors) {
        mnet->add_node(actor, layer);
    }
    // adding all edges among them
    for (NodeSharedPtr node1: *mnet->get_nodes(layer)) {
		for (NodeSharedPtr node2: *mnet->get_nodes(layer)) {
			if (node1!=node2) {
				mnet->add_edge(node1,node2);
			}
		}
    }
}

void BAEvolutionModel::internal_evolution_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer, ActorListSharedPtr& available_actors)  {
	// Choose a new actor to join the layer
    if (available_actors->size()<1) return;
    
    
    ActorSharedPtr new_actor = available_actors->get_at_random();
    available_actors->erase(new_actor);
    
    NodeSharedPtr new_node = mnet->add_node(new_actor,layer);
    
    // Randomly select m nodes with probability proportional to their degree...
    std::set<NodeSharedPtr> nodes;
    // NOTE: this operation may not be very efficient - think of an alternative implementation
    
    while (nodes.size()<m) {
        EdgeSharedPtr edge = mnet->get_edges()->get_at_random();
        nodes.insert(test(.5)?edge->v1:edge->v2);
    }

    // and connect them to the new node
    for (NodeSharedPtr old_node: nodes) {
        EdgeSharedPtr new_edge = mnet->add_edge(new_node,old_node);
    }

}
    
    void BAEvolutionModel::external_evolution_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& target_layer, ActorListSharedPtr& available_actors, const LayerSharedPtr& ext_layer)  {
        // Choose a new actor to join the layer
        if (available_actors->size()<1) return;
        
        ActorSharedPtr new_actor = available_actors->get_at_random();
        available_actors->erase(new_actor);
        
        NodeSharedPtr new_node = mnet->add_node(new_actor,target_layer);
        
        NodeSharedPtr imported_node = mnet->get_node(new_actor,ext_layer);
        
        // If the actor is not present on the layer from where we should import contacts, then we do not add any neighbors
        if (!imported_node) return;
        
        // now we insert the actor's (at most n) neighbors on ext_layer into the target layer, if they are also present there
        for (NodeSharedPtr neighbor: *mnet->neighbors(imported_node,OUT)) {
            NodeSharedPtr local_neighbor = mnet->get_node(neighbor->actor,target_layer);
            if (local_neighbor) {
                mnet->add_edge(new_node,local_neighbor);
            }
        }
    }

/*
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

} */

}
