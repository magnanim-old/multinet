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


    CommunitiesSharedPtr mlcpm(const MLNetworkSharedPtr& mnet, size_t k, size_t m1, size_t m2, size_t m3);

/**
 * This method finds all the maximal cliques (1) being a superset of clique A
 * (2) extended with actors in B (3) not containing actors in C (4) on
 * at least k actors and m layers.
 */
    hash_set<CliqueSharedPtr> find_max_cliques(const MLNetworkSharedPtr& mnet, size_t k, size_t m);

    
    hash_set<LayerSharedPtr> neighboring_layers(MLNetworkSharedPtr mnet, ActorSharedPtr actor1, ActorSharedPtr actor2);
    
}
#endif // MLNET_MLCPM_H_