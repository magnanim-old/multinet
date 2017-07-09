/*
 * rcpp_utils.cpp
 *
 * Created on: Jul 29, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "rcpp_utils.h"
#include <algorithm>

using namespace mlnet;

std::vector<LayerSharedPtr> resolve_layers(const MLNetworkSharedPtr& mnet, const CharacterVector& names) {
	std::vector<LayerSharedPtr> res(names.size()?names.size():mnet->get_layers()->size());
	if (names.size()==0) {
		int i=0;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			res[i] = layer;
			i++;
		}
	}
	else {
		for (int i=0; i<names.size(); ++i) {
			LayerSharedPtr layer = mnet->get_layer(std::string(names[i]));
			if (!layer) stop("cannot find layer " + std::string(names[i]));
			res[i] = layer;
		}
	}
	return res;
}

std::unordered_set<LayerSharedPtr> resolve_layers_unordered(const MLNetworkSharedPtr& mnet, const CharacterVector& names) {
	std::unordered_set<LayerSharedPtr> res;
	if (names.size()==0) {
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			res.insert(layer);
		}
	}
	else {
		for (int i=0; i<names.size(); ++i) {
			LayerSharedPtr layer = mnet->get_layer(std::string(names[i]));
			if (!layer) stop("cannot find layer " + std::string(names[i]));
			res.insert(layer);
		}
	}
	return res;
}


std::vector<NodeSharedPtr> resolve_nodes(const MLNetworkSharedPtr& mnet, const CharacterVector& nodes) {
	if (nodes.size()%2 != 0)
		stop("Nodes must be specified as actor/layer names (i.e., two entries for each node)");
	int num_nodes = nodes.size()/2;
	std::vector<NodeSharedPtr> res(num_nodes);
	for (int i=0; i<num_nodes; i++) {
		ActorSharedPtr actor = mnet->get_actor(std::string(nodes(i*2)));
		if (!actor) stop("cannot find actor " + std::string(nodes(i*2)));
		LayerSharedPtr layer = mnet->get_layer(std::string(nodes(i*2+1)));
		if (!layer) stop("cannot find layer " + std::string(nodes(i*2+1)));
		NodeSharedPtr node = mnet->get_node(actor,layer);
		if (!node) stop("cannot find node " + actor->name + " " + layer->name);
		res[i] = node;
	}
	return res;
}

std::vector<EdgeSharedPtr> resolve_edges(const MLNetworkSharedPtr& mnet, const CharacterVector& edges) {
	if (edges.size()%4 != 0)
		stop("Edges must be specified as pairs of actor/layer names (i.e., four entries for each edge)");
	int num_edges = edges.size()/4;
	std::vector<EdgeSharedPtr> res(num_edges);
	for (int i=0; i<num_edges; i++) {
		ActorSharedPtr actor1 = mnet->get_actor(std::string(edges(i*4)));
		if (!actor1) stop("cannot find actor " + std::string(edges(i*4)));
		ActorSharedPtr actor2 = mnet->get_actor(std::string(edges(i*4+2)));
		if (!actor2) stop("cannot find actor " + std::string(edges(i*4+2)));
		LayerSharedPtr layer1 = mnet->get_layer(std::string(edges(i*4+1)));
		if (!layer1) stop("cannot find layer " + std::string(edges(i*4+1)));
		LayerSharedPtr layer2 = mnet->get_layer(std::string(edges(i*4+3)));
		if (!layer2) stop("cannot find layer " + std::string(edges(i*4+3)));
		NodeSharedPtr node1 = mnet->get_node(actor1,layer1);
		if (!node1) stop("cannot find node " + actor1->name + " " + layer1->name);
		NodeSharedPtr node2 = mnet->get_node(actor2,layer2);
		if (!node2) stop("cannot find node " + actor2->name + " " + layer2->name);
		EdgeSharedPtr edge = mnet->get_edge(node1,node2);
		if (!edge) stop("cannot find edge " + node1->to_string() + " -> " + node2->to_string());
		res[i] = edge;
	}
	return res;
}

std::vector<ActorSharedPtr> resolve_actors(const MLNetworkSharedPtr& mnet, const CharacterVector& names) {
	std::vector<ActorSharedPtr> res(names.size()?names.size():mnet->get_actors()->size());
	if (names.size()==0) {
		int i = 0;
		for (ActorSharedPtr actor: *mnet->get_actors()) {
			res[i] = actor;
			i++;
		}
	}
	else {
		for (int i=0; i<names.size(); ++i) {
			ActorSharedPtr actor = mnet->get_actor(std::string(names[i]));
			if (!actor) stop("cannot find actor " + std::string(names[i]));
			res[i] = actor;
		}
	}
	return res;
}

std::unordered_set<ActorSharedPtr> resolve_actors_unordered(const MLNetworkSharedPtr& mnet, const CharacterVector& names) {
	std::unordered_set<ActorSharedPtr> res;
	if (names.size()==0) {
		for (ActorSharedPtr actor: *mnet->get_actors()) {
			res.insert(actor);
		}
	}
	else {
		for (int i=0; i<names.size(); ++i) {
			ActorSharedPtr actor = mnet->get_actor(std::string(names[i]));
			if (!actor) stop("cannot find actor " + std::string(names[i]));
			res.insert(actor);
		}
	}
	return res;
}

edge_mode resolve_mode(std::string mode) {
    if (mode=="all")
        return INOUT;
    else if (mode=="in")
    	return IN;
    else if (mode=="out")
    	return OUT;
    else stop("Unexpected value: edge mode " + mode);
	return INOUT; // never reaches here
}
