#include "test.h"
#include <unordered_set>
#include <string>
#include "../include/multinet.h"
#include "../include/community/pmm.h"

using namespace mlnet;

void test_pmm() {

	test_begin("ML-PMM");

	pmm p;
	MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/Downloads/DK_pol.mpx","toy",',');

	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/fftwyt.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/friendfeed_ita.mpx","sample",',');
	unsigned int k = 11 ;
	unsigned int ell = 18;
	unsigned int maxKmeans = 5;

	p.get_ml_community(mnet3, k, ell, maxKmeans);

	test_end("ML-PMM");

}


