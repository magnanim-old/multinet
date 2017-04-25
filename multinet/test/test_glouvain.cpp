#include "test.h"
#include <unordered_set>
#include <string>
#include "../include/multinet.h"
#include "../include/community/glouvain.h"

using namespace mlnet;

void test_glouvain() {

	test_begin("ML-GLOUVAIN");

	glouvain k;
	MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/Downloads/DK_pol.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/Downloads/test_1500_all.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/fftwyt.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/friendfeed_ita.mpx","sample",',');
	double gamma = 1;
	double omega = 1;

	k.get_ml_community(mnet3, gamma, omega);

	test_end("ML-GLOUVAIN");

}


