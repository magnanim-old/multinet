#include "test.h"

using namespace mlnet;

void test_community_single_layer() {

	test_begin("Label propagation, single layer");

	MLNetworkSharedPtr mnet = read_multilayer("toy.mpx","cpm",',');
    CommunityStructureSharedPtr c = label_propagation_single(mnet, mnet->get_layer("Work"));

    std::cout << c->to_string() << std::endl;

	test_end("Label propagation, single layer");

}


