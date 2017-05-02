#include "test.h"
#include <unordered_set>
#include <string>
#include "../include/multinet.h"
#include "../include/community/lart.h"

using namespace mlnet;

void test_lart() {

	test_begin("ML-LART");

	lart k;
	MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/multinet/multinet/test/toy.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/Downloads/toy.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/Downloads/toy2.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/fftwyt.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/friendfeed_ita.mpx","sample",',');
	uint32_t t = 9;
	double eps = 1;
	double gamma = 1;

	CommunitiesSharedPtr c = k.get_ml_community(mnet3, t, eps, gamma);

	std::ofstream out("/home/guest/multinet/multinet/test/DK_Pol_lart.txt");
	//(*c).print(std::cout);

	test_end("ML-LART");

}


