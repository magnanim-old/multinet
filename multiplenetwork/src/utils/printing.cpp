#include "utils.h"
#include <iostream>

using namespace mlnet;

void print(const MLNetworkSharedPtr mnet) {
	layer_list layers = mnet->get_layers();
	for (LayerSharedPtr layer : layers) {
		std::cout << "Layer: " << layer->to_string() << std::endl;
		//std::cout << "nodes:" << std::endl;
		node_list nodes = mnet->get_nodes(layer);
		for (NodeSharedPtr node : nodes) {
			std::cout << " Node: " << node->to_string() << std::endl;
		}
		//std::cout << " edges:" << std::endl;
		edge_list edges = mnet->get_edges(layer,layer);
		for (EdgeSharedPtr edge : edges) {
			std::cout << " Edge: " << edge->to_string() << std::endl;
		}
	}

	layer_list layers1 = mnet->get_layers();
	layer_list layers2 = mnet->get_layers();
	for (LayerSharedPtr layer1 : layers1) {
		for (LayerSharedPtr layer2 : layers2) {
			if (layer1==layer2) continue;
			if (mnet->num_edges(layer1,layer2)==0) continue;
			std::cout << std::endl;
			edge_list edges = mnet->get_edges(layer1,layer2);
			for (EdgeSharedPtr edge : edges) {
				std::cout << " Inter-layer Edge: " << edge->to_string() << std::endl;
			}
		}
	}
}
