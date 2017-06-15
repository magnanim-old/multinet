#include "test.h"
#include "../include/community/flattening.h"

using namespace mlnet;

void test_flattening() {

	test_begin("Flattening and detecting communities");

	MLNetworkSharedPtr mnet = read_multilayer("toy.mpx","cpm",',');


    CommunityStructureSharedPtr c = flattenAndDetectComs(mnet,ZeroOne,LabelPropagation);

    std::cout << c->to_string() << std::endl;

	test_end("Flattening and detecting communities");

}


