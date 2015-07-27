/*
 * degree.cpp
 *
 * Created on: Feb 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "measures.h"
#include <map>

using namespace std;

namespace mlnet {

ObjectStore<ActorSharedPtr> neighbors(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode) {
	ObjectStore<ActorSharedPtr> neighbors_on_selected_layers;
	for (NodeSharedPtr node: mnet->get_nodes(actor)) {
		for (NodeSharedPtr neighbor: mnet->neighbors(node, mode)) {
			if (layers.count(neighbor->layer)>0) {
				neighbors_on_selected_layers.insert(neighbor->actor->id,neighbor->actor);
			}
		}
	}
	return neighbors_on_selected_layers;
}

ObjectStore<ActorSharedPtr> neighbors(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode) {
		ObjectStore<ActorSharedPtr> neighbors_on_selected_layer;
		for (NodeSharedPtr node: mnet->get_nodes(actor)) {
			for (NodeSharedPtr neighbor: mnet->neighbors(node, mode)) {
				if (neighbor->layer==layer)
					neighbors_on_selected_layer.insert(neighbor->actor->id,neighbor->actor);
			}
		}
		return neighbors_on_selected_layer;
	}

///////////////////////////////////

	ObjectStore<ActorSharedPtr> xneighbors(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode) {
		ObjectStore<ActorSharedPtr> neighbors_on_selected_layers;
		std::set<ActorSharedPtr> neighbors_on_other_layers;
		for (NodeSharedPtr node: mnet->get_nodes(actor)) {
			for (NodeSharedPtr neighbor: mnet->neighbors(node, mode)) {
				if (layers.count(neighbor->layer)>0)
					neighbors_on_selected_layers.insert(neighbor->actor->id,neighbor->actor);
				else neighbors_on_other_layers.insert(neighbor->actor);
			}
		}
		for (ActorSharedPtr entry: neighbors_on_other_layers)
			neighbors_on_selected_layers.erase(entry->id);
		return neighbors_on_selected_layers;
	}

	ObjectStore<ActorSharedPtr> xneighbors(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode) {
		ObjectStore<ActorSharedPtr> neighbors_on_selected_layer;
		std::set<ActorSharedPtr> neighbors_on_other_layers;
		for (NodeSharedPtr node: mnet->get_nodes(actor)) {
			for (NodeSharedPtr neighbor: mnet->neighbors(node, mode)) {
				if (neighbor->layer==layer)
					neighbors_on_selected_layer.insert(neighbor->actor->id,neighbor->actor);
				else neighbors_on_other_layers.insert(neighbor->actor);
			}
		}
		for (ActorSharedPtr entry: neighbors_on_other_layers) {
			neighbors_on_selected_layer.erase(entry->id);
		}
		return neighbors_on_selected_layer;
	}

} // namespace
