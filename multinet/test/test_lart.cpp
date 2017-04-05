#include "test.h"
#include <unordered_set>
#include <string>
#include "../include/multinet.h"
#include "../include/community/lart.h"

using namespace mlnet;

void test_lart() {

	//test_begin("ML-LART");

	lart k;
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/toy2.mpx","toy",',');
	MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/Downloads/test_100_all.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/fftwyt.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/friendfeed_ita.mpx","sample",',');
	uint32_t t = 9;
	double eps = 1;
	double gamma = 1;

	k.get_ml_community(mnet3, t, eps, gamma);

	//test_end("ML-LART");

}


