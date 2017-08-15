/*
 * layer similarity.cpp
 *
 */

#include "datastructures.h"
#include "utils.h"

namespace mlnet {

property_matrix<ActorSharedPtr,LayerSharedPtr,bool> actor_existence_property_matrix(const MLNetworkSharedPtr& mnet) {
	property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P(mnet->get_actors()->size(),mnet->get_layers()->size(),false);
	for (LayerSharedPtr layer: *mnet->get_layers())
		for (NodeSharedPtr node: *mnet->get_nodes(layer)) {
			P.set(node->actor,layer,true);
		}
	return P;
}

// TODO document: does not consider self edges
property_matrix<dyad,LayerSharedPtr,bool> edge_existence_property_matrix(const MLNetworkSharedPtr& mnet) {
	long n = mnet->get_actors()->size();
	property_matrix<dyad,LayerSharedPtr,bool> P(n*(n-1)/2,mnet->get_layers()->size(),false);
	for (LayerSharedPtr layer: *mnet->get_layers()) {
		for (EdgeSharedPtr e: *mnet->get_edges(layer,layer)) {
			dyad d(e->v1->actor,e->v2->actor);
			P.set(d,layer,true);
		}
	}
	return P;
}

// only works for multiplex networks (no inter-layer edges)
property_matrix<triad,LayerSharedPtr,bool> triangle_existence_property_matrix(const MLNetworkSharedPtr& mnet) {
	long n = mnet->get_actors()->size();
	property_matrix<triad,LayerSharedPtr,bool> P(n*(n-1)*(n-2)/6,mnet->get_layers()->size(),false);
	for (LayerSharedPtr layer: *mnet->get_layers()) {
		hash_set<NodeSharedPtr> processed1;
		for (NodeSharedPtr n1: *mnet->get_nodes(layer)) {
			processed1.insert(n1);
			hash_set<NodeSharedPtr> processed2;
			for (NodeSharedPtr n2: *mnet->neighbors(n1,INOUT)) {
				if (processed1.count(n2)>0) continue;
				processed2.insert(n2);
				for (NodeSharedPtr n3: *mnet->neighbors(n2,INOUT)) {
					if (processed1.count(n3)>0) continue;
					if (processed2.count(n3)>0) continue;
					if (mnet->get_edge(n3,n1)) {
						triad t(n1->actor,n2->actor,n3->actor);
						P.set(t,layer,true);
					}
				}
			}
		}
	}
	return P;
}

property_matrix<ActorSharedPtr,LayerSharedPtr,double> actor_degree_property_matrix(const MLNetworkSharedPtr& mnet, edge_mode mode) {
	property_matrix<ActorSharedPtr,LayerSharedPtr,double> P(mnet->get_actors()->size(),mnet->get_layers()->size(),0);
	for (ActorSharedPtr actor: *mnet->get_actors()) {
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			NodeSharedPtr node = mnet->get_node(actor,layer);
			if (!node) P.set_na(actor,layer);
			else P.set(actor,layer,mnet->neighbors(node,mode)->size());
		}
	}
	return P;
}

}
