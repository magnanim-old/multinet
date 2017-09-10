/*
 * Generic data structures for community detection
 */

#include "utils.h"
#include <iostream>
#include <fstream>

#include "community.h"

namespace mlnet {

    // COMMUNITY

    community::community() {}

    CommunitySharedPtr community::create() {
        return CommunitySharedPtr(new community());
    }

    std::string community::to_string() const {
        std::string result = "";
        size_t idx = 0;
        for (NodeSharedPtr node: data) {
            if (idx==0) result += "[";
            result += node->to_string();
            if (idx!=data.size()-1) result += ", ";
            else result += "]";
            idx++;
        }
        return result;
    }

    void community::add_node(const NodeSharedPtr& node) {
        data.insert(node);
    }

    const hash_set<NodeSharedPtr>& community::get_nodes() const {
        return data;
    }

    // ACTOR COMMUNITY
    
    actor_community::actor_community() {}
    
    ActorCommunitySharedPtr actor_community::create() {
        return ActorCommunitySharedPtr(new actor_community());
    }
    
    std::string actor_community::to_string() const {
        std::string result = "";
        size_t idx = 0;
        for (ActorSharedPtr actor: actors) {
            if (idx==0) result += "[";
            result += actor->to_string();
            if (idx!=actors.size()-1) result += ", ";
            else result += "]";
            idx++;
        }
        for (LayerSharedPtr layer: layers) {
            if (idx==0) result += "::[";
            result += layer->to_string();
            if (idx!=layers.size()-1) result += ", ";
            else result += "]";
            idx++;
        }
        return result;
    }
    
    void actor_community::add_actor(ActorSharedPtr actor) {
        actors.insert(actor);
    }
    
    const hash_set<ActorSharedPtr>& actor_community::get_actors() const {
        return actors;
    }
    
    void actor_community::add_layer(LayerSharedPtr layer) {
        layers.insert(layer);
    }
    
    const hash_set<LayerSharedPtr>& actor_community::get_layers() const {
        return layers;
    }
    
    // COMMUNITIES
    
    community_structure::community_structure() {}
    
    CommunityStructureSharedPtr community_structure::create() {
        return CommunityStructureSharedPtr(new community_structure());
    }
    
    std::string community_structure::to_string() const {
        std::string result = "";
        for (CommunitySharedPtr com: data) {
            result += com->to_string() + "\n";
        }
        return result;
    }

    void community_structure::print(std::ostream& stream) {
        for (size_t i = 0; i < data.size(); i++) {
            hash_set<NodeSharedPtr> p = (*(data[i])).get_nodes();

            for (hash_set<NodeSharedPtr>::iterator it = p.begin(); it != p.end(); ++it) {
                stream << (*it)->actor->name << "," << (*it)->layer->name << "," << i << std::endl;
            }
        }
    }

    void community_structure::add_community(const CommunitySharedPtr& com) {
        data.push_back(com);
    }

    CommunitySharedPtr community_structure::get_community(int i) {
        return data.at(i);
    }

    const vector<CommunitySharedPtr>& community_structure::get_communities() const {
        return data;
    }
    

    actor_community_structure::actor_community_structure() {}
    
    ActorCommunityStructureSharedPtr actor_community_structure::create() {
        return ActorCommunityStructureSharedPtr(new actor_community_structure());
    }
    
    std::string actor_community_structure::to_string() const {
        std::string result = "";
        for (ActorCommunitySharedPtr com: data) {
            result += com->to_string() + "\n";
        }
        return result;
    }
    
    void actor_community_structure::print(std::ostream& stream) {
        /*for (size_t i = 0; i < data.size(); i++) {
            hash_set<NodeSharedPtr> p = (*(data[i])).get_nodes();
            
            for (hash_set<NodeSharedPtr>::iterator it = p.begin(); it != p.end(); ++it) {
                stream << (*it)->actor->name << "," << (*it)->layer->name << "," << i << std::endl;
            }
        }*/
        for (ActorCommunitySharedPtr comm: data) {
            stream << comm->to_string() << std::endl;
        }
    }
    
    void actor_community_structure::add_community(const ActorCommunitySharedPtr& com) {
        data.push_back(com);
    }
    
    ActorCommunitySharedPtr actor_community_structure::get_community(int i) {
        return data.at(i);
    }
    
    const vector<ActorCommunitySharedPtr>& actor_community_structure::get_communities() const {

        return data;
    }
    
    // CONVERSION
    
    CommunityStructureSharedPtr to_node_communities(const ActorCommunityStructureSharedPtr& comm, const MLNetworkSharedPtr& net) {
        CommunityStructureSharedPtr result = community_structure::create();
        for (ActorCommunitySharedPtr ac: comm->get_communities()) {
            result->add_community(to_node_community(ac,net));
        }
        return result;
    }
    
    CommunitySharedPtr to_node_community(const ActorCommunitySharedPtr& comm, const MLNetworkSharedPtr& net) {
        CommunitySharedPtr result = community::create();
        for (ActorSharedPtr actor: comm->get_actors()) {
            for (LayerSharedPtr layer: comm->get_layers()) {
                result->add_node(net->get_node(actor,layer));
            }
        }
        return result;
    }
}
