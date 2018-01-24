#include "test.h"
#include "../include/community/mlp.h"

using namespace mlnet;

void test_mlp() {

	test_begin("Testing Multi-layer Label Prppagation mlp");

	//MLNetworkSharedPtr mnet = read_multilayer("toy_ds_for_mlp.mpx","toy",',');
	//MLNetworkSharedPtr mnet = read_multilayer("toy.mpx","toy",',');
	MLNetworkSharedPtr mnet = read_multilayer("aucs.mpx","aucs",',');

    ActorCommunityStructureSharedPtr c = mlp(mnet);

    std::cout << c->to_string() << std::endl;

	test_end("End Testing Multi-layer Label Prppagation mlp");

}


