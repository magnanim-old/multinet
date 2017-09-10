#include <set>
#include <cstdlib>
#include <iostream>
#include "generation.h"

using namespace std;

namespace mlnet {

UniformEvolutionModel::UniformEvolutionModel(size_t m0) {
	UniformEvolutionModel::m0 = m0;
}

UniformEvolutionModel::~UniformEvolutionModel() {
	//
}

void UniformEvolutionModel::internal_evolution_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer, ActorListSharedPtr& available_actors) {
	// Randomly pick two nodes (uniform probability) and connect them
	NodeSharedPtr v1 = mnet->get_nodes(layer)->get_at_random();
	NodeSharedPtr v2 = mnet->get_nodes(layer)->get_at_random(); // this allows self-edges
	mnet->add_edge(v1,v2);
}

    
    void UniformEvolutionModel::external_evolution_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& target_layer, ActorListSharedPtr& available_actors, const LayerSharedPtr& ext_layer) {
        // Randomly pick an edge (uniform probability) on the external layer and connect its ends on the target (if they are present)
        if (mnet->get_edges(ext_layer,ext_layer)->size()==0) return;
        EdgeSharedPtr e = mnet->get_edges(ext_layer,ext_layer)->get_at_random();
        NodeSharedPtr v1 = mnet->get_node(e->v1->actor,target_layer);
        NodeSharedPtr v2 = mnet->get_node(e->v2->actor,target_layer);
        if (v1 && v2) mnet->add_edge(v1,v2);
    }
    
void UniformEvolutionModel::init_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer, ActorListSharedPtr& available_actors) {
    if (available_actors->size()<m0)
        throw WrongParameterException("not enough actors available to initialize the layer (less than m0)");
    std::set<ActorSharedPtr> new_actors;
    // choosing the m0 actors
    for (size_t i=0; i<m0; i++) {
        ActorSharedPtr new_actor = available_actors->get_at_random();
        new_actors.insert(new_actor);
        available_actors->erase(new_actor);
    }
    // adding the actors to the layer
    for (ActorSharedPtr actor: new_actors) {
        mnet->add_node(actor, layer);
    }
}

}



