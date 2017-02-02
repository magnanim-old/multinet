/*
 * flattening.cpp
 *
 * Created on: Mar 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "transformation.h"

const std::string SEP = "|";

namespace mlnet {

LayerSharedPtr project_unweighted(MLNetworkSharedPtr& mnet, const std::string& new_layer_name, const LayerSharedPtr& layer1, const LayerSharedPtr& layer2) {
	std::unordered_set<LayerSharedPtr> target;
	target.insert(layer1);
	LayerSharedPtr new_layer = create_layer(mnet,new_layer_name,target,false,false);
	mnet->set_directed(new_layer,new_layer,false);

	//mnet->edge_features(new_layer,new_layer)->add(DEFAULT_WEIGHT_ATTR_NAME,NUMERIC_TYPE);

	for (NodeSharedPtr node: *mnet->get_nodes(layer2)) {
		for (NodeSharedPtr n1: *mnet->neighbors(node,IN)) {
			if (n1->layer!=layer1) continue;
			NodeSharedPtr target_node1 = mnet->get_node(n1->actor,new_layer);
			for (NodeSharedPtr n2: *mnet->neighbors(node,IN)) {
				if (n2->layer!=layer1) continue;
				NodeSharedPtr target_node2 = mnet->get_node(n2->actor,new_layer);
				if (n1<n2) mnet->add_edge(target_node1,target_node2);
			}
		}
	}
	return new_layer;
}

}

