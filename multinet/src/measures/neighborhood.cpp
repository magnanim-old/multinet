/*
 * neighbors
 *
 * Created on: Feb 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "measures.h"
#include <map>

using namespace std;

namespace mlnet {
    
    ActorListSharedPtr neighbors(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const std::unordered_set<LayerSharedPtr>& layers, edge_mode mode) {
        ActorListSharedPtr neighbors_on_selected_layers = std::make_shared<actor_list>();
        for (NodeSharedPtr node: *mnet->get_nodes(actor)) {
            for (NodeSharedPtr neighbor: *mnet->neighbors(node, mode)) {
                if (layers.count(neighbor->layer)>0) {
                    neighbors_on_selected_layers->insert(neighbor->actor);
                }
            }
        }
        return neighbors_on_selected_layers;
    }
    
    ActorListSharedPtr neighbors(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode) {
        ActorListSharedPtr neighbors_on_selected_layer = std::make_shared<actor_list>();
        for (NodeSharedPtr node: *mnet->get_nodes(actor)) {
            for (NodeSharedPtr neighbor: *mnet->neighbors(node, mode)) {
                if (neighbor->layer==layer)
                    neighbors_on_selected_layer->insert(neighbor->actor);
            }
        }
        return neighbors_on_selected_layer;
    }
    
    ///////////////////////////////////
    
    ActorListSharedPtr xneighbors(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const std::unordered_set<LayerSharedPtr>& layers, edge_mode mode) {
        ActorListSharedPtr neighbors_on_selected_layers = std::make_shared<actor_list>();
        std::set<ActorSharedPtr> neighbors_on_other_layers;
        for (NodeSharedPtr node: *mnet->get_nodes(actor)) {
            for (NodeSharedPtr neighbor: *mnet->neighbors(node, mode)) {
                if (layers.count(neighbor->layer)>0)
                    neighbors_on_selected_layers->insert(neighbor->actor);
                else neighbors_on_other_layers.insert(neighbor->actor);
            }
        }
        for (ActorSharedPtr entry: neighbors_on_other_layers)
            neighbors_on_selected_layers->erase(entry);
        return neighbors_on_selected_layers;
    }
    
    ActorListSharedPtr xneighbors(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const LayerSharedPtr& layer, edge_mode mode) {
        ActorListSharedPtr neighbors_on_selected_layer = std::make_shared<actor_list>();
        std::set<ActorSharedPtr> neighbors_on_other_layers;
        for (NodeSharedPtr node: *mnet->get_nodes(actor)) {
            for (NodeSharedPtr neighbor: *mnet->neighbors(node, mode)) {
                if (neighbor->layer==layer)
                    neighbors_on_selected_layer->insert(neighbor->actor);
                else neighbors_on_other_layers.insert(neighbor->actor);
            }
        }
        for (ActorSharedPtr entry: neighbors_on_other_layers) {
            neighbors_on_selected_layer->erase(entry);
        }
        return neighbors_on_selected_layer;
    }
    
    double connective_redundancy(const MLNetworkSharedPtr& mnet, const ActorSharedPtr& actor, const hash_set<LayerSharedPtr>& layers, edge_mode mode) {
        double d = degree(mnet,actor,layers,mode);
        if (d==0) return 0;
        return 1 - neighbors(mnet,actor,layers,mode)->size()/d;
    }
    
} // namespace
