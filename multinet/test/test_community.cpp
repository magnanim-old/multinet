/*
 * testCommunity.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "test.h"
#include <unordered_set>
#include <string>

using namespace mlnet;



void test_community() {

	test_begin("community data structures");

	MLNetworkSharedPtr mnet = read_multilayer("aucs.mpx","aucs",',');

    ActorSharedPtr a1 = mnet->get_actor("U54");
    ActorSharedPtr a2 = mnet->get_actor("U4");
	LayerSharedPtr l1 = mnet->get_layer("lunch");
	LayerSharedPtr l2 = mnet->get_layer("leisure");

	CommunitySharedPtr c1_1 = community::create();
	c1_1->add_node(mnet->get_node(a1,l1));
	c1_1->add_node(mnet->get_node(a1,l2));
    CommunitySharedPtr c1_2 = community::create();
    c1_2->add_node(mnet->get_node(a2,l1));
    c1_2->add_node(mnet->get_node(a2,l2));
	CommunityStructureSharedPtr com1 = community_structure::create();
	com1->add_community(c1_1);
	com1->add_community(c1_2);

    CommunitySharedPtr c2_1 = community::create();
    c2_1->add_node(mnet->get_node(a1,l1));
    c2_1->add_node(mnet->get_node(a1,l2));
    c2_1->add_node(mnet->get_node(a2,l1));
    CommunitySharedPtr c2_2 = community::create();
    c2_2->add_node(mnet->get_node(a2,l2));
    CommunityStructureSharedPtr com2 = community_structure::create();
    com2->add_community(c2_1);
    com2->add_community(c2_2);

    CommunitySharedPtr c3_1 = community::create();
    c3_1->add_node(mnet->get_node(a1,l1));
    c3_1->add_node(mnet->get_node(a2,l1));
    CommunitySharedPtr c3_2 = community::create();
    c3_2->add_node(mnet->get_node(a1,l2));
    c3_2->add_node(mnet->get_node(a2,l2));
    CommunityStructureSharedPtr com3 = community_structure::create();
    com3->add_community(c3_1);
    com3->add_community(c3_2);

    //std::cout << com->to_string() << std::endl;

    std::cout << "Testing community comparison function...";
    if (std::abs(community_jaccard(c1_1,c3_1)-1.0/3)>.01) throw FailedUnitTestException("Wrong community_jaccard function: " + to_string(community_jaccard(c1_1,c3_1)));
    std::cout << "done!" << std::endl;

    std::cout << "Testing community comparison function...";
    std::cout << normalized_mutual_information(com1,com1,4) << " ";
    std::cout << normalized_mutual_information(com1,com2,4) << " ";
    std::cout << normalized_mutual_information(com1,com3,4) << " ";
    std::cout << "done!" << std::endl;
    CommunityStructureSharedPtr truth = read_ground_truth("ground_truth.dat",',',mnet);
    
    std::cout << "Same info, read from file - it should give the same results...";
    std::cout << normalized_mutual_information(truth,com1,4) << " ";
    std::cout << normalized_mutual_information(truth,com2,4) << " ";
    std::cout << normalized_mutual_information(truth,com3,4) << " ";
    std::cout << "done!" << std::endl;
    
    MLNetworkSharedPtr cpm = read_multilayer("florentine_families.mpx","toy",',');

    std::cout << "Running algorithms..."<< std::endl;
    std::cout << "====================="<< std::endl;
    std::cout << "ABACUS"<< std::endl;
    int min_actors = 3;
    int min_layers = 1;
    ActorCommunityStructureSharedPtr communities = abacus(cpm, min_actors, min_layers);
    std::cout << "actor-version"<< std::endl;
    std::cout << communities->to_string();
    std::cout << "node-version" << std::endl;
    CommunityStructureSharedPtr n_communities = to_node_communities(communities,cpm);
    std::cout << n_communities->to_string();
    
    std::cout << "=====================" << std::endl;
    std::cout << "LART"<< std::endl;
    lart k; uint32_t t = 9; double eps = 1; double gamma = 1;
    n_communities = k.fit(cpm, t, eps, gamma);
    std::cout << n_communities->to_string();

    
    std::cout << "====================="<< std::endl;
    std::cout << "GLOUVAIN" << std::endl;
    glouvain gl;
    double l_gamma = 1.0;
    double l_omega = 1.0;
    double l_limit = 10000;
    std::string move = "move";
    
    n_communities = gl.fit(cpm, move, l_gamma, l_omega, l_limit);
    std::cout << n_communities->to_string();
    
    std::cout << "=====================" << std::endl;
    std::cout << "ML-CPM" << std::endl;
    size_t cpm_k = 3;
    size_t cpm_m = 1;
    n_communities = mlcpm(cpm, cpm_k, cpm_m);
    std::cout << n_communities->to_string();
    
    std::cout << "=====================" << std::endl;
    std::cout << "Flattening" << std::endl;
    communities = flattenAndDetectComs(cpm,ZeroOne,LabelPropagation);
    std::cout << communities->to_string();
    
    //std::cout << "modularity (NOTE: not defined for overlapping communities): ";
    //std::cout << modularity(toy,n_communities,1) << std::endl;
    //std::cout << "done!" << std::endl;
    
	test_end("community data structures");
}
