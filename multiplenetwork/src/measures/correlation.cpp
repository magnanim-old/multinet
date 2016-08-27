/*
 * layer similarity.cpp
 *
 */

#include "datastructures.h"
#include "utils.h"


namespace mlnet {

property_matrix<ActorSharedPtr,LayerSharedPtr,bool> actor_existence_property_matrix(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P(mnet->get_actors().size(),mnet->get_layers().size(),false);
	for (NodeSharedPtr node: mnet->get_nodes(layer1)) {
		P.set(node->actor,layer1,true);
	}
	for (NodeSharedPtr node: mnet->get_nodes(layer2)) {
		P.set(node->actor,layer2,true);
	}
	return P;
}

property_matrix<dyad,LayerSharedPtr,bool> edge_existence_property_matrix(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	long n = mnet->get_actors().size();
	property_matrix<dyad,LayerSharedPtr,bool> P(n*(n-1)/2+n,mnet->get_layers().size(),false);
	for (EdgeSharedPtr e: mnet->get_edges(layer1,layer1)) {
		dyad d(e->v1->actor,e->v2->actor);
		P.set(d,layer1,true);
	}
	for (EdgeSharedPtr e: mnet->get_edges(layer2,layer2)) {
		dyad d(e->v1->actor,e->v2->actor);
		P.set(d,layer2,true);
	}
	return P;
}

double jaccard_actor(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet,layer1,layer2);
	return jaccard(P,layer1,layer2);
}

double jaccard_edge(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet,layer1,layer2);
	return jaccard(P,layer1,layer2);
}

// TODO DOC: only undirected
double jaccard_triangle(const MLNetworkSharedPtr& mnet, const simple_set<LayerSharedPtr>& layers) {
	TriangleCounter<actor_id,actor_id,actor_id> c;
	int num_layers = layers.size();
	long num_triangles = 0;
	long num_matches = 0;
	// count all the triangles
	for (LayerSharedPtr layer: layers) {
		std::unordered_set<NodeSharedPtr> processed1;
		for (NodeSharedPtr n1: mnet->get_nodes(layer)) {
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

double pearson_degree(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2, edge_mode mode) {
	property_matrix<ActorSharedPtr,LayerSharedPtr,long> P(mnet->get_actors().size(),mnet->get_layers().size(),0);
	for (NodeSharedPtr node: mnet->get_nodes(layer1)) {
		P.set(node->actor,layer1,mnet->neighbors(node,mode).size());
	}
	for (NodeSharedPtr node: mnet->get_nodes(layer2)) {
		P.set(node->actor,layer2,mnet->neighbors(node,mode).size());
	}
	return pearson(P,layer1,layer2);
}

double rho_degree(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2, edge_mode mode) {
	property_matrix<ActorSharedPtr,LayerSharedPtr,long> P(mnet->get_actors().size(),mnet->get_layers().size(),0);
	for (NodeSharedPtr node: mnet->get_nodes(layer1)) {
		P.set(node->actor,layer1,mnet->neighbors(node,mode).size());
	}
	P.rankify(layer1);
	for (NodeSharedPtr node: mnet->get_nodes(layer2)) {
		P.set(node->actor,layer2,mnet->neighbors(node,mode).size());
	}
	P.rankify(layer2);
	return pearson(P,layer1,layer2);
}

}
