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

	actor_list neighbors(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode) {
		map<actor_id,ActorSharedPtr> neighbors_on_selected_layers;
		node_list nodes = mnet->get_nodes(actor);
		for (NodeSharedPtr node: nodes) {
			node_list neighbors = mnet->neighbors(node, mode);
			for (NodeSharedPtr neighbor: neighbors) {
				if (layers.count(neighbor->layer)>0)
					neighbors_on_selected_layers[neighbor->actor->id] = neighbor->actor;
			}
		}
		return actor_list(neighbors_on_selected_layers);
	}

	actor_list neighbors(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode) {
		map<actor_id,ActorSharedPtr> neighbors_on_selected_layers;
		node_list nodes = mnet->get_nodes(actor);
		for (NodeSharedPtr node: nodes) {
			node_list neighbors = mnet->neighbors(node, mode);
			for (NodeSharedPtr neighbor: neighbors) {
				if (neighbor->layer==layer)
					neighbors_on_selected_layers[neighbor->actor->id] = neighbor->actor;
			}
		}
		return actor_list(neighbors_on_selected_layers);
	}

///////////////////////////////////

	actor_list xneighbors(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode) {
		map<actor_id,ActorSharedPtr> neighbors_on_selected_layers;
		map<actor_id,ActorSharedPtr> neighbors_on_other_layers;
		node_list nodes = mnet->get_nodes(actor);
		for (NodeSharedPtr node: nodes) {
			node_list neighbors = mnet->neighbors(node, mode);
			for (NodeSharedPtr neighbor: neighbors) {
				if (layers.count(neighbor->layer)>0)
					neighbors_on_selected_layers[neighbor->actor->id] = neighbor->actor;
				else neighbors_on_other_layers[neighbor->actor->id] = neighbor->actor;
			}
		}
		for (pair<actor_id,ActorSharedPtr> entry: neighbors_on_other_layers)
			neighbors_on_selected_layers.erase(entry.first);
		return actor_list(neighbors_on_selected_layers);
	}

	actor_list xneighbors(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode) {
		map<actor_id,ActorSharedPtr> neighbors_on_selected_layers;
		map<actor_id,ActorSharedPtr> neighbors_on_other_layers;
		node_list nodes = mnet->get_nodes(actor);
		for (NodeSharedPtr node: nodes) {
			node_list neighbors = mnet->neighbors(node, mode);
			for (NodeSharedPtr neighbor: neighbors) {
				if (neighbor->layer==layer)
					neighbors_on_selected_layers[neighbor->actor->id] = neighbor->actor;
				else neighbors_on_other_layers[neighbor->actor->id] = neighbor->actor;
			}
		}
		for (pair<actor_id,ActorSharedPtr> entry: neighbors_on_other_layers)
			neighbors_on_selected_layers.erase(entry.first);
		return actor_list(neighbors_on_selected_layers);
	}

} // namespace
