#include "test.h"
#include <unordered_set>
#include <string>
#include "../include/multinet.h"
#include "../include/community/pmm.h"

using namespace mlnet;

void test_pmm() {

	test_begin("ML-PMM");

	pmm p;
	MLNetworkSharedPtr mnet = read_multilayer("/home/guest/multinet-evaluation/data/aucs","aucs", ',');

	std::vector<int> k = {5, 10, 20, 40, 60, 100};
	double gamma = 1.5;

	for (size_t i = 0; i < k.size(); i++) {
		CommunityStructureSharedPtr c = p.fit(mnet, k[i], k[i] * 2, gamma);
		//for (size_t j = 0; i < ell.size(); j++) {
		//}
	}



	test_end("ML-PMM");
}


