#include "test.h"
#include <unordered_set>
#include <string>
#include "../include/multinet.h"

using namespace mlnet;

void test_lart() {

	test_begin("ML-LART");

	lart k;
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/toy2.mpx","toy",',');
	MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/test_200_all.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/fftwyt.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/friendfeed_ita.mpx","sample",',');
	uint32_t t = 9;
	float eps = 1;
	float gamma = 1;

	k.get_ml_community(mnet3, t, eps, gamma);

	test_end("ML-LART");

}


