#include "test.h"
#include <unordered_set>
#include <string>
#include "../include/community/lart.h"
#include "../include/community/abacus.h"

using namespace mlnet;

mlnet::CommunityStructureSharedPtr read_truth2(mlnet::MLNetworkSharedPtr mnet) {

	std::fstream myfile("/home/guest/multinet-evaluation/truth/1k_mix01", std::ios_base::in);
	int actor;
	int community;

	mlnet::hash_map<long,std::set<mlnet::NodeSharedPtr> > result;

	while (myfile >> actor) {
		myfile >> community;
		mlnet::ActorSharedPtr a = mnet->get_actor(std::to_string(actor));
		for (mlnet::LayerSharedPtr l: *mnet->get_layers()) {
			result[community].insert(mnet->get_node(a,l));
		}
	}

	mlnet::CommunityStructureSharedPtr communities = mlnet::community_structure::create();

	for (auto pair: result) {
		mlnet::CommunitySharedPtr c = mlnet::community::create();
		for (mlnet::NodeSharedPtr node: pair.second) {
			c->add_node(node);
		}
		communities->add_community(c);
	}

	return communities;
}


void test_lart() {

	test_begin("ML-LART");


	lart k;
	//MLNetworkSharedPtr mnet = read_multilayer("/home/guest/multinet-evaluation/data/1k_mix01","toy",' ');
	//MLNetworkSharedPtr mnet = read_multilayer("/home/guest/multinet-evaluation/data/1k_mix01","toy",' ');
	MLNetworkSharedPtr mnet = read_multilayer("/home/guest/multinet-evaluation/data/1k_mix01","toy",' ');

	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/multinet-evaluation/data/aucs","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/multinet-evaluation/data/fftwyt","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("../data/aucs.mpx","aucs",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/1k_mix_025.mpx","sample",',');
	uint32_t t = 9;
	double eps = 1;
	double gamma = 1;

	CommunityStructureSharedPtr c = k.fit(mnet, t, eps, gamma);
	CommunityStructureSharedPtr truth = read_truth2(mnet);

	std::cout << modularity(mnet, c, gamma) << std::endl;
	std::cout << normalized_mutual_information(c, truth, mnet->get_nodes()->size()) << std::endl;


	//std::ofstream out("/home/guest/multinet/multinet/test/DK_Pol_lart.txt");
	//(*c).print(std::cout);

	test_end("ML-LART");

}


