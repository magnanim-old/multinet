/*
 * Generic data structures for community detection
 */

#include "utils.h"
#include <iostream>

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
    
    void community::add_node(NodeSharedPtr node) {
        data.insert(node);
    }
    
    const hash_set<NodeSharedPtr>& community::get_nodes() const {
        return data;
    }
   
    // COMMUNITIES
    
    communities::communities() {}
    
    CommunitiesSharedPtr communities::create() {
        return CommunitiesSharedPtr(new communities());
    }
    
    std::string communities::to_string() const {
        std::string result = "";
        for (CommunitySharedPtr com: data) {
            result += com->to_string() + "\n";
        }
        return result;
    }
    
    void communities::add_community(CommunitySharedPtr com) {
        data.push_back(com);
    }
    
    CommunitySharedPtr communities::get_community(int i) {
        return data.at(i);
    }
    
    const vector<CommunitySharedPtr>& communities::get_communities() const {
        return data;
    }
}
