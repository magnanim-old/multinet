/**
 * ml-cpm.h
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: 1.0
 *
 * Multi-layer clique percolation method.
 */

#ifndef MLNET_MLCPM_H_
#define MLNET_MLCPM_H_

#include "datastructures.h"

namespace mlnet {

    class cpm_community;
    typedef std::shared_ptr<cpm_community> AdjCliqueCommunitySharedPtr;
    
    // DATA STRUCTURES
    /* */
    struct layer_set_comparator {
        bool operator()(const sorted_set<LayerSharedPtr>& a, const sorted_set<LayerSharedPtr>& b) const {
            if (a.size() != b.size()) return a.size() < b.size();
            sorted_set<LayerSharedPtr>::iterator it1 = a.begin();
            sorted_set<LayerSharedPtr>::iterator it2 = b.begin();
            for (size_t i=0; i<a.size(); i++) {
                if ((*it1)<(*it2))
                    return true;
                if ((*it1)>(*it2))
                    return false;
                ++it1; ++it2;
            }
            return false;
        }
    };
    typedef std::set<sorted_set<LayerSharedPtr>,layer_set_comparator > layer_sets;
    
    /* the ML-CPM algorithm uses a special type of community, defined as a set of adjacent cliques */
    class cpm_community {
    public:
        cpm_community();
        cpm_community(long id, hash_set<CliqueSharedPtr> cliques, hash_set<LayerSharedPtr> layers);
    
        static AdjCliqueCommunitySharedPtr create();
        
        void add_clique(CliqueSharedPtr);
        void add_layer(LayerSharedPtr);
        const std::set<LayerSharedPtr>& get_layers();
        std::set<ActorSharedPtr> actors() const;
        CommunitySharedPtr to_community(const MLNetworkSharedPtr& net) const;
        int size() const;
        bool operator==(const cpm_community& comp) const;
        bool operator!=(const cpm_community& comp) const;
        bool operator<(const cpm_community& comp) const;
        bool operator>(const cpm_community& comp) const;
        std::string to_string();
        
        long id;
        std::set<CliqueSharedPtr> cliques;
        std::set<LayerSharedPtr> layers;
    };
    
    
    //
    

    CommunityStructureSharedPtr mlcpm(const MLNetworkSharedPtr& mnet, size_t k, size_t m);

/**
 * This method finds all the maximal cliques (1) being a superset of clique A
 * (2) extended with actors in B (3) not containing actors in C (4) on
 * at least k actors and m layers.
 */
    hash_set<CliqueSharedPtr> find_max_cliques(const MLNetworkSharedPtr& mnet, size_t k, size_t m);
    
    hash_set<LayerSharedPtr> neighboring_layers(MLNetworkSharedPtr mnet, ActorSharedPtr actor1, ActorSharedPtr actor2);
    
    std::map<CliqueSharedPtr,hash_set<CliqueSharedPtr> > build_max_adjacency_graph(const hash_set<CliqueSharedPtr>& C, size_t k, size_t m);
    
    hash_set<AdjCliqueCommunitySharedPtr> find_max_communities(MLNetworkSharedPtr mnet, const std::map<CliqueSharedPtr,hash_set<CliqueSharedPtr> >& adjacency, size_t m);
    
    void find_max_communities(const std::map<CliqueSharedPtr,hash_set<CliqueSharedPtr> >& adjacency, AdjCliqueCommunitySharedPtr& A,
                                         vector<CliqueSharedPtr> Candidates, hash_set<CliqueSharedPtr>& processedCliques, layer_sets& processedLayerCombinations, size_t m, hash_set<AdjCliqueCommunitySharedPtr>& result);
}
#endif // MLNET_MLCPM_H_
