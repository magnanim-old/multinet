/*
 * Unit testing: random.h
 */
#include "test.h"
#include <unordered_set>
#include <string>
#include "../include/multinet.h"
#include "../include/community/singleLayer/singleLayer.h"

using namespace mlnet;



void test_walktrap() {

	test_begin("walktrap algorithm");

	MLNetworkSharedPtr mnet = read_multilayer("data/aucs.mpx","aucs",',');

	LayerSharedPtr layer = mnet->get_layer("lunch");

	CommunitiesSharedPtr coms = SingleLayer::walkTrap(mnet,layer);
    
    if (coms ==NULL)
    {
    	throw FailedUnitTestException("Algorithm returened 0 communities");
    }

	test_end("walktrap algorithm");
}
