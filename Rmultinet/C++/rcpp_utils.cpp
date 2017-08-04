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


std::vector<NodeSharedPtr> resolve_nodes(const MLNetworkSharedPtr& mnet, const DataFrame& nodes) {
	std::vector<NodeSharedPtr> res(nodes.nrow());
    CharacterVector a = nodes(0);
    CharacterVector l = nodes(1);
    
    for (int i=0; i<nodes.nrow(); i++) {
        ActorSharedPtr actor = mnet->get_actor(std::string(a(i)));
        if (!actor) stop("cannot find actor " + std::string(a(i)));
        LayerSharedPtr layer = mnet->get_layer(std::string(l(i)));
        if (!layer) stop("cannot find layer " + std::string(l(i)));
        NodeSharedPtr node = mnet->get_node(actor,layer);
        if (!node) stop("cannot find actor " + actor->name + " on layer " + layer->name);
        res[i] = node;
    }
	return res;
}

std::vector<EdgeSharedPtr> resolve_edges(const MLNetworkSharedPtr& mnet, const DataFrame& edges) {
	std::vector<EdgeSharedPtr> res(edges.nrow());
    CharacterVector a_from = edges(0);
    CharacterVector l_from = edges(1);
    CharacterVector a_to = edges(2);
    CharacterVector l_to = edges(3);
    
    for (int i=0; i<edges.nrow(); i++) {
        ActorSharedPtr actor1 = mnet->get_actor(std::string(a_from(i)));
        if (!actor1) stop("cannot find actor " + std::string(a_from(i)));
        ActorSharedPtr actor2 = mnet->get_actor(std::string(a_to(i)));
        if (!actor2) stop("cannot find actor " + std::string(a_to(i)));
        LayerSharedPtr layer1 = mnet->get_layer(std::string(l_from(i)));
        if (!layer1) stop("cannot find layer " + std::string(l_from(i)));
        LayerSharedPtr layer2 = mnet->get_layer(std::string(l_to(i)));
        if (!layer2) stop("cannot find layer " + std::string(l_to(i)));
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

DataFrame to_dataframe(CommunityStructureSharedPtr cs) {
    
    CharacterVector actor, layer;
    NumericVector community_id;
    
    int comm_id=0;
    for (CommunitySharedPtr com: cs->get_communities()) {
        for (NodeSharedPtr node: com->get_nodes()) {
            actor.push_back(node->actor->name);
            layer.push_back(node->layer->name);
            community_id.push_back(comm_id);
        }
        comm_id++;
    }
    return DataFrame::create(_("actor")=actor,_("layer")=layer,_("cid")=community_id);
}
