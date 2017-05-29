#include "test.h"
#include <unordered_set>
#include <string>
#include "../include/multinet.h"
#include "../include/community/glouvain.h"

using namespace mlnet;

void test_glouvain() {

	test_begin("ML-GLOUVAIN");

	glouvain k;
	MLNetworkSharedPtr mnet = read_multilayer("test/toy.mpx", "aucs",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/multinet-evaluation/data/monastery","aucs",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/multinet-evaluation/data/1k_mix","toy",',');

	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/Downloads/10k_all.txt","toy",' ');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/multinet-evaluation/data/fftwyt","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/friendfeed_ita.mpx","sample",',');
	double gamma = 1.0;
	double omega = 1.0;
	std::string move = "move";

	//(*(k.fit(mnet, move, gamma, omega))).print(std::cout);

	test_end("ML-GLOUVAIN");

}


