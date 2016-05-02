#include "utils.h"
#include <iostream>

using namespace mlnet;

void print(const MLNetworkSharedPtr mnet) {
	for (LayerSharedPtr layer : mnet->get_layers()) {
		std::cout << "Layer: " << layer->to_string() << std::endl;
		//std::cout << "nodes:" << std::endl;
		for (NodeSharedPtr node : mnet->get_nodes(layer)) {
			std::cout << " Node: " << node->to_string() << std::endl;
		}
		//std::cout << " edges:" << std::endl;
		for (EdgeSharedPtr edge : mnet->get_edges(layer,layer)) {
			std::cout << " Edge: " << edge->to_string() << std::endl;
		}
	}

	for (LayerSharedPtr layer1 : mnet->get_layers()) {
		for (LayerSharedPtr layer2 : mnet->get_layers()) {
			if (layer1==layer2) continue;
			if (mnet->get_edges(layer1,layer2).size()==0) continue;
			std::cout << std::endl;
			for (EdgeSharedPtr edge : mnet->get_edges(layer1,layer2)) {
				std::cout << " Inter-layer Edge: " << edge->to_string() << std::endl;
			}
		}
	}
}
