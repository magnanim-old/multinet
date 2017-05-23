/*
 * Test file for clique percolation algorithm
 *
 * Author: matteomagnani
 */

#include "test.h"

using namespace mlnet;

void test_community_cpm() {

	test_begin("community detection: clique percolation");

	MLNetworkSharedPtr mnet = read_multilayer("aucs.mpx","aucs",',');

	CommunityStructureSharedPtr comm = mlcpm(mnet, 3, 1);
    
    for (CommunitySharedPtr c: comm->get_communities()) {
        for (NodeSharedPtr node: c->get_nodes())
            std::cout << "(" << node->actor->name << "," << node->layer->name << ") ";
        std::cout << std::endl;
    }
    
	test_end("community data structures");
}
