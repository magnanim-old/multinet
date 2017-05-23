#include "test.h"
#include <unordered_set>
#include <string>
#include "../include/community/lart.h"
#include "../include/community/abacus.h"

using namespace mlnet;

void test_lart() {

	test_begin("ML-LART");

    
	lart k;
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/multinet/multinet/test/toy.mpx","toy",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/Downloads/3k_mix025_all.txt","toy",' ');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/guest/multinet-evaluation/data/5k_mix025","toy",' ');
	MLNetworkSharedPtr mnet3 = read_multilayer("aucs.mpx","aucs",',');
	//MLNetworkSharedPtr mnet3 = read_multilayer("/home/mikki/Downloads/1k_mix_025.mpx","sample",',');
	uint32_t t = 2;
	double eps = 1;
	double gamma = 1;

	CommunityStructureSharedPtr c = k.fit(mnet3, t, eps, gamma);

    
    ActorSharedPtr a1 = mnet3->get_actor("U4");
    ActorSharedPtr a2 = mnet3->get_actor("U54");
    ActorSharedPtr a3 = mnet3->get_actor("U18");
    LayerSharedPtr l1 = mnet3->get_layer("lunch");
    LayerSharedPtr l2 = mnet3->get_layer("leisure");
    NodeSharedPtr n1_1 = mnet3->get_node(a1,l1);
    NodeSharedPtr n1_2 = mnet3->get_node(a1,l2);
    NodeSharedPtr n2_1 = mnet3->get_node(a2,l1);
    NodeSharedPtr n2_2 = mnet3->get_node(a2,l2);
    NodeSharedPtr n3_1 = mnet3->get_node(a3,l1);
    NodeSharedPtr n3_2 = mnet3->get_node(a3,l2);
    CommunitySharedPtr c1_1 = community::create();
    c1_1->add_node(n1_1);
    c1_1->add_node(n2_1);
    CommunitySharedPtr c1_2 = community::create();
    c1_2->add_node(n3_1);
    CommunityStructureSharedPtr com1 = community_structure::create();
    com1->add_community(c1_1);
    com1->add_community(c1_2);
    
    
    CommunitySharedPtr c2_1 = community::create();
    c2_1->add_node(n1_1);
    c2_1->add_node(n2_1);
    CommunitySharedPtr c2_2 = community::create();
    c2_2->add_node(n2_1);
    c2_2->add_node(n3_1);
    CommunityStructureSharedPtr com2 = community_structure::create();
    com2->add_community(c2_1);
    com2->add_community(c2_2);

    vector<CommunityStructureSharedPtr> communities = {com1, com2};
    abacus(mnet3, communities, 1);
    
	//std::ofstream out("/home/guest/multinet/multinet/test/DK_Pol_lart.txt");
	//(*c).print(std::cout);

	test_end("ML-LART");

}


