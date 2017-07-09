#include "utils.h"
#include "community.h"

#include <cstdio>
#include <stdio.h>
#include <iostream>
#include <fstream>

namespace mlnet {
    
    CommunityStructureSharedPtr to_community_structure(hash_map<NodeSharedPtr,int> membership) {
        CommunityStructureSharedPtr result = community_structure::create();
        hash_map<int,hash_set<NodeSharedPtr> > communities;
        for (auto pair: membership) {
            communities[pair.second].insert(pair.first);
        }
        for (auto pair: communities) {
            CommunitySharedPtr c = community::create();
            for (NodeSharedPtr n: pair.second)
                c->add_node(n);
            result->add_community(c);
        }
        return result;
    }

    CommunityStructureSharedPtr label_propagation_single(const MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer) {
        
        NodeListSharedPtr nodes = mnet->get_nodes(layer);
        hash_map<NodeSharedPtr,int> membership; // community membership
        std::vector<NodeSharedPtr> order; // order vector to decide in which order to process the nodes
        // Initialize labels
        int l=0;
        for (NodeSharedPtr node: *nodes) {
            membership[node] = l;
            order.push_back(node);
            l++;
        }
        
        while (true) {
            
            /* Compute order of node processing */
            std::random_shuffle(order.begin(), order.end());
            
            /* re-assign labels */
            for (NodeSharedPtr node: order) {
                Counter<int> num_neighbors;
                for (NodeSharedPtr neigh: *mnet->neighbors(node,INOUT)) { // TODO make it also for directed graphs?
                    num_neighbors.inc(membership.at(neigh));
                }
                membership[node] = num_neighbors.max();
            }
            
            /* Check stopping condition */
            for (NodeSharedPtr node: order) {
                NodeListSharedPtr neighbors = mnet->neighbors(node,INOUT);
                Counter<int> num_neighbors;
                for (NodeSharedPtr neigh: *neighbors) {
                    num_neighbors.inc(membership.at(neigh));
                }
                if (num_neighbors.count(membership.at(node)) != num_neighbors.count(num_neighbors.max())) continue;
            }
            
            break;
    }
    
    // Build community structure
    return to_community_structure(membership);
    
    }
}
