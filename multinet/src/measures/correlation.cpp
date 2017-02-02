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
			if (!node) P.set(actor,layer,0);
			else P.set(actor,layer,mnet->neighbors(node,mode)->size());
		}
	}
	return P;
}

/*
double jaccard_actor(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet,layer1,layer2);
	return jaccard(P,layer1,layer2);
}

double jaccard_edge(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet,layer1,layer2);
	return jaccard(P,layer1,layer2);
}

double jaccard_triangle(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	property_matrix<trad,LayerSharedPtr,bool> P = triangle_existence_property_matrix(mnet,layer1,layer2);
	return jaccard(P,layer1,layer2);
}


// TODO DOC: only undirected
double jaccard_triangle(const MLNetworkSharedPtr& mnet, const hash_set<LayerSharedPtr>& layers) {
	TriangleCounter<actor_id,actor_id,actor_id> c;
	int num_layers = layers.size();
	long num_triangles = 0;
	long num_matches = 0;
	// count all the triangles
	for (LayerSharedPtr layer: layers) {
		std::unordered_set<NodeSharedPtr> processed1;
		for (NodeSharedPtr n1: *mnet->get_nodes(layer)) {
			processed1.insert(n1);
			std::unordered_set<NodeSharedPtr> processed2;
			for (NodeSharedPtr n2: mnet->neighbors(n1,INOUT)) {
				if (processed1.count(n2)>0) continue;
				processed2.insert(n2);
				for (NodeSharedPtr n3: mnet->neighbors(n2,INOUT)) {
					if (processed1.count(n3)>0) continue;
					if (processed2.count(n3)>0) continue;
					if (mnet->get_edge(n3,n1)) {
						std::vector<actor_id> triangle(3);
						triangle[0] = n1->actor->id;
						triangle[1] = n2->actor->id;
						triangle[2] = n3->actor->id;
						std::sort(triangle.begin(),triangle.end());
						c.inc(triangle[0],triangle[1],triangle[2]);
					}
				}
			}
		}
	}
	for (auto& e1: c.map()) {
		for (auto& e2: e1.second) {
			for (auto& e3: e2.second) {
				num_triangles++;
				if (e3.second==num_layers)
					num_matches++;
			}
		}
	}
	return double(num_matches)/num_triangles;
}

double jaccard_triangle(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	std::unordered_set<LayerSharedPtr> layers;
	layers.insert(layer1);
	layers.insert(layer2);
	return jaccard_triangle(mnet,layers);
}

double coverage_actor(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet,layer1,layer2);
	return coverage(P,layer1,layer2);
}

double coverage_edge(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet,layer1,layer2);
	return coverage(P,layer1,layer2);
}

double simple_matching_actor(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet,layer1,layer2);
	return simple_matching(P,layer1,layer2);
}

double simple_matching_edge(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet,layer1,layer2);
	return simple_matching(P,layer1,layer2);
}

double russell_rao_actor(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet,layer1,layer2);
	return russell_rao(P,layer1,layer2);
}

double russell_rao_edge(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet,layer1,layer2);
	return russell_rao(P,layer1,layer2);
}

double kulczynski2_actor(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet,layer1,layer2);
	return kulczynski2(P,layer1,layer2);
}

double kulczynski2_edge(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet,layer1,layer2);
	return kulczynski2(P,layer1,layer2);
}

*/

}
