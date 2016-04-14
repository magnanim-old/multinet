#include "utils.h"
#include "randomwalks.h"
#include <vector>

namespace mlnet {

Walker::Walker(const MLNetworkSharedPtr mlnet, double teleportation, const matrix<double>& transitions)
: mlnet(mlnet), teleportation(teleportation), transitions(transitions), just_teleported(true), no_action(true) {
	current = mlnet->get_nodes().get_at_random();
	int i=0;
	for (LayerSharedPtr layer: mlnet->get_layers()) {
		layer_idx[layer->id] = i;
		i++;
	}
}

Walker::~Walker() {}

NodeSharedPtr Walker::now() {
	return current;
}

NodeSharedPtr Walker::next() {
 	if (test(teleportation)) {
 		current = mlnet->get_nodes().get_at_random();
 		just_teleported = true;
 		no_action = false;
 	}
 	else { // random step
 		int lidx = layer_idx.at(current->layer->id);
 		int layer_idx = test(transitions, lidx);
 		LayerSharedPtr new_layer = mlnet->get_layers().get_at_index(layer_idx);
 		if (current->layer==new_layer) {
 			// Moving inside the same layer
 			sorted_set<node_id,NodeSharedPtr> neigh = mlnet->neighbors(current,OUT);
 			if (neigh.size()==0) {
 				// No possibility to move: no action
 		 		no_action = true;
 		 		return current;
 			}
 			long rand = getRandomInt(neigh.size());
			current = neigh.get_at_index(rand);
			just_teleported = false;
	 		no_action = false;
			//log("--> " + next->to_string());
		}
 		else {
 			// Changing node associated to the same actor
 			NodeSharedPtr next_node = mlnet->get_node(current->actor,new_layer);
 			if (!next_node) {
 				// No other nodes for this actor: no action
 		 		no_action = true;
 		 		return current;
 			}
 			current = next_node;
 			just_teleported = false;
 	 		no_action = false;
 		}
	}
	return current;
}

bool Walker::teleported() {
	return just_teleported;
}

bool Walker::action() {
	return !no_action;
}

} // Namespace mlnet

