#include "test.h"
#include "../include/community/flattening.h"


using namespace mlnet;

void test_flattening() {

	test_begin("Flattening and detecting communities");

	MLNetworkSharedPtr mnet = read_multilayer("dkpol.mpx","dkpol",',');

	std::cout << "before flattening : " <<std::endl << mnet->to_string() <<std::endl;

	MLNetworkSharedPtr fnet = flatten(mnet,NumOfLayers);

	std::cout << "before flattening : " <<std::endl << fnet->to_string() <<std::endl;


	LayerSharedPtr fLayer = fnet->get_layer("flattened");
	CommunityStructureSharedPtr coms  = label_propagation_single(fnet, fLayer);
	std::cout << "communities using label propagation : " <<std::endl << coms->to_string() <<std::endl;

	std::cout <<std::endl;
	std::cout <<std::endl;
	std::cout <<std::endl;
	std::cout <<std::endl;
	std::cout <<std::endl;
	std::cout <<std::endl;
	std::cout <<std::endl;

	CommunityStructureSharedPtr result = map_back_to_ml(coms,mnet);
	std::cout << "after_mapping " <<std::endl << result->to_string() <<std::endl;
	test_end("Flattening and detecting communities");

}


