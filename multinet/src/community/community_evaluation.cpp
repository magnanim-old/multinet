/*
 * Evaluation functions for community detection
 */

#include "community.h"

namespace mlnet {

    double community_jaccard(const CommunitySharedPtr& c1, const CommunitySharedPtr& c2) {
        int common_nodes = intersection_size(c1->get_nodes(),c2->get_nodes());
        return (double)common_nodes/(c1->get_nodes().size()+c2->get_nodes().size()-common_nodes);
    }

    /**
     * Returns the agreement between two clusterings com1 and com2
     * @param com1
     * @param com2
     * @param n number of nodes in the original network
     */
    double normalized_mutual_information(const CommunityStructureSharedPtr& com1, const CommunityStructureSharedPtr& com2, int n) {
        double entropy_c1 = 0;

        for (size_t i = 0; i < com1->get_communities().size(); i++) {
            int size1 = com1->get_community(i)->get_nodes().size();
            if (size1 == 0) {
                continue;
            }
            entropy_c1 -= (double)size1/n * std::log2((double)size1/n);
        }

        double entropy_c2 = 0;
        for (size_t j = 0; j < com2->get_communities().size(); j++) {
            int size2 = com2->get_community(j)->get_nodes().size();
            if (size2 == 0){
              continue;
            }
            entropy_c2 -= (double)size2/n * std::log2((double)size2/n);
        }

        double info = 0;
        for (size_t i = 0; i < com1->get_communities().size(); i++) {
            for (size_t j = 0; j < com2->get_communities().size(); j++) {

                int common_nodes = intersection_size(com1->get_community(i)->get_nodes(),com2->get_community(j)->get_nodes());

                int size1 = com1->get_community(i)->get_nodes().size();
                int size2 = com2->get_community(j)->get_nodes().size();

                if (size1==0 || size2==0 || common_nodes==0)
                    continue;
                info += (double)common_nodes/n * std::log2((double)n*common_nodes/(size1*size2));
            }
        }
        return info/((entropy_c1+entropy_c2)/2);
    }
    
    CommunityStructureSharedPtr read_ground_truth(const string& infile, char separator, const MLNetworkSharedPtr& mnet) {
        CSVReader csv;
        csv.trimFields(true);
        csv.setFieldSeparator(separator);
        csv.open(infile);
        hash_map<std::string, std::set<NodeSharedPtr> > result;
        while (csv.hasNext()) {
            vector<string> v = csv.getNext();
            ActorSharedPtr a = mnet->get_actor(v.at(0));
            LayerSharedPtr l = mnet->get_layer(v.at(1));
            NodeSharedPtr n = mnet->get_node(a,l);
            result[v.at(2)].insert(n);
            
        }
        
        CommunityStructureSharedPtr communities = community_structure::create();
        
        for (auto pair: result) {
            CommunitySharedPtr c = community::create();
            for (NodeSharedPtr node: pair.second) {
                c->add_node(node);
            }
            communities->add_community(c);
        }
        
        return communities;
    }

}
