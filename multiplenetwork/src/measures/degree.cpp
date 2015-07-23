/*
 * degree.cpp
 *
 * Created on: Feb 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "measures.h"
#include "utils.h"
#include <vector>
#include <iostream>

namespace mlnet {

long degree(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode) {
	int degree = 0;
	node_list nodes = mnet->get_nodes(actor);
	for (NodeSharedPtr node: nodes) {
		node_list neighbors = mnet->neighbors(node, mode);
		for (NodeSharedPtr neighbor: neighbors) {
			if (layers.count(neighbor->layer)>0)
				degree += 1;
		}
	}
	return degree;
}

long degree(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode) {
	int degree = 0;
	node_list nodes = mnet->get_nodes(actor);
	for (NodeSharedPtr node: nodes) {
		node_list neighbors = mnet->neighbors(node, mode);
		for (NodeSharedPtr neighbor: neighbors) {
			if (neighbor->layer==layer)
				degree += 1;
		}
	}
	return degree;
}

double degree_mean(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode) {
	std::vector<double> degrees;
	for (LayerSharedPtr layer: layers) {
		degrees.push_back((double)degree(mnet,actor,layer,mode));
	}
	return mean(degrees);
}

double degree_deviation(const MLNetworkSharedPtr mnet, const ActorSharedPtr& actor, const std::set<LayerSharedPtr>& layers, edge_mode mode) {
	std::vector<double> degrees;
	for (LayerSharedPtr layer: layers) {
		degrees.push_back((double)degree(mnet,actor,layer,mode));
	}
	return stdev(degrees);
}
} // Namespace mlnet

