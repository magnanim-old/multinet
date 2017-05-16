#include "test.h"
#include <unordered_set>
#include <string>
#include "../include/multinet.h"
#include "../include/community/pmm.h"

using namespace mlnet;

void test_pmm() {

	test_begin("ML-PMM");

	pmm p;
	MLNetworkSharedPtr mnet = read_multilayer("../data/aucs.mpx","aucs",',');

	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/fftwyt.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/friendfeed_ita.mpx","sample",',');
	unsigned int k = 5;
	unsigned int ell = 3;
	p.fit(mnet, k, ell);
	test_end("ML-PMM");
}


