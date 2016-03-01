/*
 * testNetwork.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "test.h"
#include "mlnetwork.h"
#include <iostream>
#include <unordered_set>
#include <string>

using namespace mlnet;



void test_community() {


	test_begin("ML-CPM");

	MLNetworkSharedPtr mnet = read_multilayer("test/toy.mpx","cpm net",',');

	std::set<CliqueSharedPtr> C = find_max_cliques(mnet,3,1);

	for (CliqueSharedPtr c: C) {
		std::cout << c->to_string() << std::endl;
	}

	std::map<CliqueSharedPtr,std::set<CliqueSharedPtr> > adjacency = build_adjacency_graph(C, 1);


	std::set<CommunitySharedPtr> communities = find_communities(mnet,adjacency,1);


	for (CommunitySharedPtr c: communities) {
		std::cout << c->to_string() << std::endl;
	}
	/*
	// We need to read the network from a file: testIO() must have been passed
	log("TESTING community detection");
	log("Reading the network...",false);
	// Creating an empty multiple network and initializing it
	MLNetworkSharedPtr mnet_a = read_multilayer("/Users/matteomagnani/Dropbox/Research/Archive/13NetworkScience/code/fig6a.mnet","6a",',');
	MLNetworkSharedPtr mnet_b = read_multilayer("/Users/matteomagnani/Dropbox/Research/Archive/13NetworkScience/code/fig6b.mnet","6b",',');

	//mnet_read_edgelist(mnet, "test/toy.mnet");

	log("done!");

	log("Computing communities...");
	hash<NodeSharedPtr,long> membership = label_propagation(mnet_a,1);


	double mod = modularity(mnet,groups_toy,1);
	log(std::to_string(mod) + " ",false);
	log("done 1 - toy!");


	for (LayerSharedPtr layer: mnet_a->get_layers()) {
		for (NodeSharedPtr node: mnet_a->get_nodes(layer))
		log(layer->name + " " + node->actor->name + " -> " + to_string(membership.at(node)));
	}

	log("GR1!");
	double mod = modularity(mnet_a,membership,0);
	log(std::to_string(mod) + " ",false);
	log("done 0!");
	mod = modularity(mnet_a,membership,.5);
	log(std::to_string(mod) + " ",false);
	log("done .5!");
	mod = modularity(mnet_a,membership,1);
	log(std::to_string(mod) + " ",false);
	log("done 1!");

	log("GR2!");
	mod = modularity(mnet_b,groups2,0);
	log(std::to_string(mod) + " ",false);
	log("done 0!");
	mod = modularity(mnet_b,groups2,.5);
	log(std::to_string(mod) + " ",false);
	log("done .5!");
	mod = modularity(mnet_b,groups2,1);
	log(std::to_string(mod) + " ",false);
	log("done 1!");
	log("GR3!");
	mod = modularity(mnet_b,groups3,0);
	log(std::to_string(mod) + " ",false);
	log("done 0!");
	mod = modularity(mnet_b,groups3,.5);
	log(std::to_string(mod) + " ",false);
	log("done .5!");
	mod = modularity(mnet_b,groups3,1);
	log(std::to_string(mod) + " ",false);
	log("done 1!");
	log("GR4!");
	mod = modularity(mnet_b,groups4,0);
	log(std::to_string(mod) + " ",false);
	log("done 0!");
	mod = modularity(mnet_b,groups4,.5);
	log(std::to_string(mod) + " ",false);
	log("done .5!");
	mod = modularity(mnet_b,groups4,1);
	log(std::to_string(mod) + " ",false);
	log("done 1!");
	 */
	log("TEST SUCCESSFULLY COMPLETED (community)");
}
