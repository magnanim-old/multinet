#include "test.h"
#include <unordered_set>
#include <string>
#include "../include/community/lart.h"

using namespace mlnet;

void test_lart() {

	test_begin("ML-LART");

	lart k;
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/multinet/multinet/test/toy.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/Downloads/3k_mix025_all.txt","toy",' ');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/multinet-evaluation/data/5k_mix025","toy",' ');
	MLNetworkSharedPtr mnet3 = read_multilayer("../data/aucs.mpx","aucs",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/1k_mix_025.mpx","sample",',');
	uint32_t t = 2;
	double eps = 1;
	double gamma = 1;

	CommunityStructureSharedPtr c = k.fit(mnet3, t, eps, gamma);

	//std::ofstream out("/home/guest/multinet/multinet/test/DK_Pol_lart.txt");
	//(*c).print(std::cout);

	test_end("ML-LART");

}


