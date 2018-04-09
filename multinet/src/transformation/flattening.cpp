/*
 * flattening.cpp
 *
 * Created on: Mar 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "transformation.h"
#include "utils.h"

const std::string SEP = "|";

namespace mlnet {

LayerSharedPtr flatten_weighted(MLNetworkSharedPtr& mnet, const std::string& new_layer_name, const std::unordered_set<LayerSharedPtr>& layers, bool force_directed, bool force_actors) {
	LayerSharedPtr new_layer = create_layer(mnet,new_layer_name,layers,force_directed,force_actors);

	bool directed = mnet->is_directed(new_layer,new_layer);
	mnet->edge_features(new_layer,new_layer)->add(DEFAULT_WEIGHT_ATTR_NAME,NUMERIC_TYPE);

	for (LayerSharedPtr layer1: layers ) {
		for (LayerSharedPtr layer2: layers) {
			for (EdgeSharedPtr edge: *mnet->get_edges(layer1,layer2)) {
				NodeSharedPtr node1 = mnet->get_node(edge->v1->actor,new_layer);
				NodeSharedPtr node2 = mnet->get_node(edge->v2->actor,new_layer);
				EdgeSharedPtr new_edge = mnet->get_edge(node1,node2);
				if (!new_edge) {
					new_edge = mnet->add_edge(node1,node2);
					mnet->set_weight(node1,node2,1);
				}
				else {
					double weight = mnet->get_weight(node1,node2);
					mnet->set_weight(node1,node2,weight+1);
				}
				// if the resulting layer is directed, undirected edges must be inserted as two directed ones
				if (directed && !edge->directionality) {
					new_edge = mnet->get_edge(node2,node1);
					if (!new_edge) {
						new_edge = mnet->add_edge(node2,node1);
						mnet->set_weight(node2,node1,1);
					}
					else {
						double weight = mnet->get_weight(node2,node1);
						mnet->set_weight(node2,node1,weight+1);
					}
				}
			}
		}
	}
	return new_layer;
}

LayerSharedPtr flatten_unweighted(MLNetworkSharedPtr& mnet, const std::string& new_layer_name, const std::unordered_set<LayerSharedPtr>& layers, bool force_directed, bool force_actors) {
	LayerSharedPtr new_layer = create_layer(mnet,new_layer_name,layers,force_directed,force_actors);

	bool directed = mnet->is_directed(new_layer,new_layer);

	for (LayerSharedPtr layer1: layers) {
		for (LayerSharedPtr layer2: layers) {
			for (EdgeSharedPtr edge: *mnet->get_edges(layer1,layer2)) {
				NodeSharedPtr node1 = mnet->get_node(edge->v1->actor,new_layer);
				NodeSharedPtr node2 = mnet->get_node(edge->v2->actor,new_layer);
				EdgeSharedPtr new_edge = mnet->get_edge(node1,node2);
				if (!new_edge) {
					new_edge = mnet->add_edge(node1,node2);
				}
				// if the resulting layer is directed, undirected edges must be inserted as two directed ones
				if (directed && !edge->directionality) {
					new_edge = mnet->get_edge(node2,node1);
					if (!new_edge) {
						new_edge = mnet->add_edge(node2,node1);
					}
				}
			}
		}
	}
	return new_layer;
}

   
    /** Only to be used for the ASONAM experiment */
    
    LayerSharedPtr flatten_obaida(MLNetworkSharedPtr& mnet, const std::string& new_layer_name, const std::unordered_set<LayerSharedPtr>& layers, bool force_directed, bool force_actors, double threshold) {
        LayerSharedPtr new_layer = create_layer(mnet,new_layer_name,layers,force_directed,force_actors);
        
        bool directed = mnet->is_directed(new_layer,new_layer);
        
        mnet->edge_features(new_layer,new_layer)->add(DEFAULT_WEIGHT_ATTR_NAME,NUMERIC_TYPE);
        mnet->edge_features(new_layer,new_layer)->add("layers",STRING_TYPE);
        
        std::unordered_map<ActorSharedPtr, std::unordered_set<std::string> > a_l;
        
        for (ActorSharedPtr actor: *mnet->get_actors()) {
            a_l[actor];
            mnet->add_node(actor, new_layer);
            
            for (LayerSharedPtr layer: layers) {
                if (mnet->get_node(actor,layer)) {
                    a_l[actor].insert(layer->name);
                }
            }
        }
        
        for (ActorSharedPtr actor1: *mnet->get_actors()) {
            for (ActorSharedPtr actor2: *mnet->get_actors()) {
                std::vector<std::unordered_set<std::string> > arr;
                arr.push_back(a_l[actor1]);
                arr.push_back(a_l[actor2]);
                double jaccard = jaccard_similarity(arr);
                if (jaccard<threshold) continue;
                
                NodeSharedPtr node1 = mnet->get_node(actor1,new_layer);
                NodeSharedPtr node2 = mnet->get_node(actor2,new_layer);
                EdgeSharedPtr new_edge = mnet->get_edge(node1,node2);
                if (!new_edge) {
                    new_edge = mnet->add_edge(node1,node2);
                    mnet->set_weight(node1,node2,jaccard);
                    std::string common_layers = "";
                    for (std::string s: a_l[actor1])
                        if (a_l[actor2].count(s)>0)
                            common_layers = common_layers + ":" + s;
                    
                    
                    mnet->edge_features(new_layer,new_layer)->setString(new_edge->id,"layers",common_layers);
                }
            }
        }
        return new_layer;
    }

    
}

