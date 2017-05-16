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
}
