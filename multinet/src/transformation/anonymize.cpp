/*
 * flattening.cpp
 *
 * Created on: Mar 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "transformation.h"

namespace mlnet {

MLNetworkSharedPtr anonymize_actors(const MLNetworkSharedPtr& mnet, const string& name) {
	MLNetworkSharedPtr anonym_net = MLNetwork::create(name);
	// generate random ids
	vector<string> names(mnet->get_actors()->size());
	for (size_t i = 0; i<names.size(); i++) {
		names[i] = "A" + to_string(i);
	}
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle (names.begin(), names.end(), std::default_random_engine(seed));
	// map actors
	hash_map<ActorSharedPtr,int> map(mnet->get_actors()->size());
	int i=0;
	for (ActorSharedPtr actor: *mnet->get_actors()) {
		map[actor] = i;
		i++;
	}

	for (LayerSharedPtr layer: *mnet->get_layers()) {
		anonym_net->add_layer(layer->name,mnet->is_directed(layer,layer)?DIRECTED:UNDIRECTED);
	}
	for (LayerSharedPtr layer1: *mnet->get_layers()) {
		for (LayerSharedPtr layer2: *mnet->get_layers()) {
			if (layer1==layer2)
				continue;
			anonym_net->set_directed(anonym_net->get_layer(layer1->name),anonym_net->get_layer(layer2->name),mnet->is_directed(layer1,layer2));
		}
	}

	for (AttributeSharedPtr attr: mnet->actor_features()->attributes()) {
		anonym_net->actor_features()->add(attr->name(),attr->type());
	}

	for (LayerSharedPtr layer: *mnet->get_layers()) {
		for (AttributeSharedPtr attr: mnet->node_features(layer)->attributes()) {
			anonym_net->node_features(anonym_net->get_layer(layer->name))->add(attr->name(),attr->type());
		}
	}

	for (LayerSharedPtr layer1: *mnet->get_layers()) {
		for (LayerSharedPtr layer2: *mnet->get_layers()) {
			for (AttributeSharedPtr attr: mnet->edge_features(layer1,layer2)->attributes()) {
				anonym_net->edge_features(anonym_net->get_layer(layer1->name),anonym_net->get_layer(layer2->name))->add(attr->name(),attr->type());
			}
		}
	}

	for (ActorSharedPtr actor: *mnet->get_actors()) {
		ActorSharedPtr new_actor = anonym_net->add_actor(names.at(map.at(actor)));
		AttributeStoreSharedPtr actor_attrs = mnet->actor_features();
		AttributeStoreSharedPtr new_actor_attrs = anonym_net->actor_features();
		for (AttributeSharedPtr attr: actor_attrs->attributes()) {
			switch (attr->type()) {
			case NUMERIC_TYPE:
				new_actor_attrs->setNumeric(new_actor->id,attr->name(),actor_attrs->getNumeric(actor->id,attr->name()));
				break;
			case STRING_TYPE:
				new_actor_attrs->setString(new_actor->id,attr->name(),actor_attrs->getString(actor->id,attr->name()));
				break;
			}
		}
	}

	for (LayerSharedPtr layer: *mnet->get_layers()) {
		for (NodeSharedPtr node: *mnet->get_nodes(layer)) {
			NodeSharedPtr new_node = anonym_net->add_node(anonym_net->get_actor(names.at(map.at(node->actor))),anonym_net->get_layer(node->layer->name));
			AttributeStoreSharedPtr node_attrs = mnet->node_features(layer);
			AttributeStoreSharedPtr new_node_attrs = anonym_net->node_features(anonym_net->get_layer(layer->name));
			for (AttributeSharedPtr attr: node_attrs->attributes()) {
				switch (attr->type()) {
				case NUMERIC_TYPE:
					new_node_attrs->setNumeric(new_node->id,attr->name(),node_attrs->getNumeric(node->id,attr->name()));
					break;
				case STRING_TYPE:
					new_node_attrs->setString(new_node->id,attr->name(),node_attrs->getString(node->id,attr->name()));
					break;
				}
			}
		}
	}

	for (LayerSharedPtr layer1: *mnet->get_layers()) {
		for (LayerSharedPtr layer2: *mnet->get_layers()) {
			for (EdgeSharedPtr edge: *mnet->get_edges(layer1,layer2)) {
				NodeSharedPtr new_node1 = anonym_net->get_node(anonym_net->get_actor(names.at(map.at(edge->v1->actor))),anonym_net->get_layer(edge->v1->layer->name));
				NodeSharedPtr new_node2 = anonym_net->get_node(anonym_net->get_actor(names.at(map.at(edge->v2->actor))),anonym_net->get_layer(edge->v2->layer->name));
				EdgeSharedPtr new_edge = anonym_net->add_edge(new_node1,new_node2);

				AttributeStoreSharedPtr edge_attrs = mnet->edge_features(layer1,layer2);
				AttributeStoreSharedPtr new_edge_attrs = anonym_net->edge_features(anonym_net->get_layer(layer1->name),anonym_net->get_layer(layer2->name));
				for (AttributeSharedPtr attr: edge_attrs->attributes()) {
					switch (attr->type()) {
					case NUMERIC_TYPE:
						new_edge_attrs->setNumeric(new_edge->id,attr->name(),edge_attrs->getNumeric(edge->id,attr->name()));
						break;
					case STRING_TYPE:
						new_edge_attrs->setString(new_edge->id,attr->name(),edge_attrs->getString(edge->id,attr->name()));
						break;
					}
				}
			}
		}
	}
	return anonym_net;
}
}

