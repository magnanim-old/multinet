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
	// log("TESTING measures");
	// log("Reading the network...",false);
	// Creating an empty multiple network and initializing it
	MLNetworkSharedPtr mnet = MLNetwork::create("test");

	ActorSharedPtr a1 = mnet->add_actor("A1");
	ActorSharedPtr a2 = mnet->add_actor("A2");
	ActorSharedPtr a3 = mnet->add_actor("A3");

	LayerSharedPtr l1 = mnet->add_layer("L1",UNDIRECTED);
	LayerSharedPtr l2 = mnet->add_layer("L2",UNDIRECTED);

	NodeSharedPtr n1 = mnet->add_node(a1,l1);
	NodeSharedPtr n2 = mnet->add_node(a1,l2);
	NodeSharedPtr n3 = mnet->add_node(a2,l1);
	NodeSharedPtr n4 = mnet->add_node(a2,l2);
	NodeSharedPtr n5 = mnet->add_node(a3,l1);
	NodeSharedPtr n6 = mnet->add_node(a3,l2);

	mnet->add_edge(n1,n3);
	mnet->add_edge(n3,n5);
	mnet->add_edge(n2,n4);
	mnet->add_edge(n4,n6);
	mnet->add_edge(n2,n6);

	//log("Computing modularity...");
	hash_map<NodeSharedPtr,long> groups;

    groups[n1] = 0;
    groups[n2] = 0;
    groups[n3] = 0;
    groups[n4] = 0;
    groups[n5] = 1;
    groups[n6] = 0;

	double mod = modularity(mnet,groups,1);

	std::cout << "Modularity: " << mod << std::endl;

	//log("TEST SUCCESSFULLY COMPLETED (modularity)");
}
