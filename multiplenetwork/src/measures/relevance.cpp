/*
 * degree.cpp
 *
 * Created on: Feb 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "measures.h"
#include <set>
#include <iostream>

using namespace std;

namespace mlnet {

double relevance(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode) {
	set<actor_id> neighbors_on_selected_layers;
	set<actor_id> all_neighbors;
	node_list nodes = mnet->get_nodes(actor);
	for (NodeSharedPtr node: nodes) {
		node_list neighbors = mnet->neighbors(node, mode);
		for (NodeSharedPtr neighbor: neighbors) {
			all_neighbors.insert(neighbor->actor->id);
			if (layers.count(neighbor->layer)>0)
				neighbors_on_selected_layers.insert(neighbor->actor->id);
		}
	}
	if (all_neighbors.size()==0) return 0; // by definition
	else return (double)neighbors_on_selected_layers.size()/all_neighbors.size();
}


double relevance(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode) {
	set<actor_id> neighbors_on_selected_layers;
	set<actor_id> all_neighbors;
	node_list nodes = mnet->get_nodes(actor);
	for (NodeSharedPtr node: nodes) {
		node_list neighbors = mnet->neighbors(node, mode);
		for (NodeSharedPtr neighbor: neighbors) {
			all_neighbors.insert(neighbor->actor->id);
			if (layer == neighbor->layer)
				neighbors_on_selected_layers.insert(neighbor->actor->id);
		}
	}
	if (all_neighbors.size()==0) return 0; // by definition
	else return (double)neighbors_on_selected_layers.size()/all_neighbors.size();
}


double xrelevance(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode) {
	set<actor_id> neighbors_on_selected_layers;
	set<actor_id> neighbors_on_other_layers;
	set<actor_id> all_neighbors;
	node_list nodes = mnet->get_nodes(actor);
	for (NodeSharedPtr node: nodes) {
		node_list neighbors = mnet->neighbors(node, mode);
		for (NodeSharedPtr neighbor: neighbors) {
			all_neighbors.insert(neighbor->actor->id);
			if (layers.count(neighbor->layer)>0)
				neighbors_on_selected_layers.insert(neighbor->actor->id);
			else neighbors_on_other_layers.insert(neighbor->actor->id);
		}
	}
	if (all_neighbors.size()==0) return 0; // by definition
	else {
		for (actor_id actor: neighbors_on_other_layers)
			neighbors_on_selected_layers.erase(actor);
		return (double)neighbors_on_selected_layers.size()/all_neighbors.size();
	}
}

double xrelevance(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode) {
	set<actor_id> neighbors_on_selected_layers;
	set<actor_id> neighbors_on_other_layers;
	set<actor_id> all_neighbors;
	node_list nodes = mnet->get_nodes(actor);
	for (NodeSharedPtr node: nodes) {
		node_list neighbors = mnet->neighbors(node, mode);
		for (NodeSharedPtr neighbor: neighbors) {
			all_neighbors.insert(neighbor->actor->id);
			if (layer==neighbor->layer)
				neighbors_on_selected_layers.insert(neighbor->actor->id);
			else neighbors_on_other_layers.insert(neighbor->actor->id);
		}
	}
	if (all_neighbors.size()==0) return 0; // by definition
	else {
		for (actor_id actor: neighbors_on_other_layers)
			neighbors_on_selected_layers.erase(actor);
		return (double)neighbors_on_selected_layers.size()/all_neighbors.size();
	}
}

} // namespace
