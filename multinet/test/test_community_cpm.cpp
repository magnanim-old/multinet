/*
 * Test file for clique percolation algorithm
 *
 * Author: matteomagnani
 */

#include "test.h"

using namespace mlnet;

void test_community_cpm() {

	test_begin("community detection: clique percolation");

	MLNetworkSharedPtr mnet = read_multilayer("cpm.mpx","aucs",',');

	CommunitiesSharedPtr comm = mlcpm(mnet, 3, 1);
    
    for (CommunitySharedPtr clique: cliques) {
        for (ActorSharedPtr actor: clique->actors)
            std::cout << actor->name << " ";
        std::cout << std::endl;
    }
    
	test_end("community data structures");
}
