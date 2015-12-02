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
#include <string>

using namespace mlnet;

void test_modularity() {

	// We need to read the network from a file: testIO() must have been passed
	log("TESTING measures");
	log("Reading the network...",false);
	// Creating an empty multiple network and initializing it
	MLNetworkSharedPtr mnet = read_multilayer("test/toy.mpx","toy",',');

	MLNetworkSharedPtr mnet_a = read_multilayer("/Users/matteomagnani/Dropbox/Research/Archive/13NetworkScience/code/fig6a.mnet","6a",',');
	MLNetworkSharedPtr mnet_b = read_multilayer("/Users/matteomagnani/Dropbox/Research/Archive/13NetworkScience/code/fig6b.mnet","6b",',');

	//mnet_read_edgelist(mnet, "test/toy.mnet");

	log("done!");

	log("Computing modularity...");
	hash<NodeSharedPtr,long> groups_toy, groups1, groups2, groups3, groups4;


	/*double mod = modularity(mnet,groups_toy,1);
	log(std::to_string(mod) + " ",false);
	log("done 1 - toy!");
	*/

	LayerSharedPtr l1 = mnet_a->get_layer("l1");
	LayerSharedPtr l2 = mnet_a->get_layer("l2");
	LayerSharedPtr l3 = mnet_a->get_layer("l3");

	ActorSharedPtr v1 = mnet_a->get_actor("v1");
	ActorSharedPtr v2 = mnet_a->get_actor("v2");
	ActorSharedPtr v3 = mnet_a->get_actor("v3");
	ActorSharedPtr v4 = mnet_a->get_actor("v4");
	ActorSharedPtr v5 = mnet_a->get_actor("v5");
	ActorSharedPtr v6 = mnet_a->get_actor("v6");
	ActorSharedPtr v7 = mnet_a->get_actor("v7");
	ActorSharedPtr v8 = mnet_a->get_actor("v8");

	groups1[mnet_a->get_node(v1,l1)] = 0;
	groups1[mnet_a->get_node(v2,l1)] = 0;
	groups1[mnet_a->get_node(v3,l1)] = 0;
	groups1[mnet_a->get_node(v4,l1)] = 1;
	groups1[mnet_a->get_node(v5,l1)] = 1;
	groups1[mnet_a->get_node(v7,l1)] = 1;
	groups1[mnet_a->get_node(v8,l1)] = 1;
	groups1[mnet_a->get_node(v1,l2)] = 0;
	groups1[mnet_a->get_node(v2,l2)] = 0;
	groups1[mnet_a->get_node(v3,l2)] = 0;
	groups1[mnet_a->get_node(v4,l2)] = 1;
	groups1[mnet_a->get_node(v5,l2)] = 1;
	groups1[mnet_a->get_node(v6,l2)] = 1;
	groups1[mnet_a->get_node(v7,l2)] = 1;
	groups1[mnet_a->get_node(v8,l2)] = 1;
	groups1[mnet_a->get_node(v1,l3)] = 0;
	groups1[mnet_a->get_node(v2,l3)] = 0;
	groups1[mnet_a->get_node(v3,l3)] = 0;
	groups1[mnet_a->get_node(v4,l3)] = 1;
	groups1[mnet_a->get_node(v5,l3)] = 1;
	groups1[mnet_a->get_node(v6,l3)] = 1;
	groups1[mnet_a->get_node(v7,l3)] = 1;
	groups1[mnet_a->get_node(v8,l3)] = 1;

	log("GR1!");
	double mod = modularity(mnet_a,groups1,0);
	log(std::to_string(mod) + " ",false);
	log("done 0!");
	mod = modularity(mnet_a,groups1,.5);
	log(std::to_string(mod) + " ",false);
	log("done .5!");
	mod = modularity(mnet_a,groups1,1);
	log(std::to_string(mod) + " ",false);
	log("done 1!");

	log("TEST SUCCESSFULLY COMPLETED (modularity)");
}
