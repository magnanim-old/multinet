#include "test.h"
#include <unordered_set>
#include <string>
#include "../include/multinet.h"
#include "../include/community/pmm.h"

using namespace mlnet;

void test_pmm() {

	test_begin("ML-PMM");

	pmm p;
	MLNetworkSharedPtr mnet = read_multilayer("/home/guest/multinet-evaluation/data/3k_mix025","aucs", ' ');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/fftwyt.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/friendfeed_ita.mpx","sample",',');

	std::vector<int> k = {5, 10, 20, 40, 60, 100};

	for (size_t i = 0; i < k.size(); i++) {
		std::vector<int> ell = {1, k[i] / 2, k[i], k[i] + 2, k[i] * 2};

		CommunityStructureSharedPtr c = p.fit(mnet, k[i], k[i] * 2);
		//for (size_t j = 0; i < ell.size(); j++) {
		//}
	}



	test_end("ML-PMM");
}


