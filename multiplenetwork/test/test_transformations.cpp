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

using namespace mlnet;

void test_transformations() {
	test_begin("flattening");

	std::cout << "Reading the multilayer network...";
	MLNetworkSharedPtr mnet = read_multilayer("test/io2.mpx","mlnet 2",',');
	ActorSharedPtr u1 = mnet->get_actor("U1");
	ActorSharedPtr u2 = mnet->get_actor("U2");
	ActorSharedPtr u3 = mnet->get_actor("U3");
	std::unordered_set<LayerSharedPtr> layers;
	layers.insert(mnet->get_layer("l1"));
	layers.insert(mnet->get_layer("l2"));
	std::cout << "done!" << std::endl;

	std::cout << "Testing weighted flattening...";
	LayerSharedPtr new_layer = flatten_weighted(mnet,"flat_weighted",layers,false,false);
	if (!mnet->is_directed(new_layer,new_layer)) throw FailedUnitTestException("Layer should be directed");
	if (mnet->get_nodes(new_layer).size() != 6) throw FailedUnitTestException("Wrong number of nodes");
	if (mnet->get_edges(new_layer,new_layer).size() != 10) throw FailedUnitTestException("Wrong number of edges");
	NodeSharedPtr n1 = mnet->get_node(u1,new_layer);
	NodeSharedPtr n2 = mnet->get_node(u2,new_layer);
	NodeSharedPtr n3 = mnet->get_node(u3,new_layer);
	if (mnet->get_weight(n1,n2) != 1) throw FailedUnitTestException("Wrong weight, expected 1");
	if (mnet->get_weight(n1,n3) != 2) throw FailedUnitTestException("Wrong weight, expected 2");
	std::cout << "done!" << std::endl;

	std::cout << "Testing or flattening...";
	new_layer = flatten_or(mnet,"flat_or",layers,false,false);
	if (!mnet->is_directed(new_layer,new_layer)) throw FailedUnitTestException("Layer should be directed");
	if (mnet->get_nodes(new_layer).size() != 6) throw FailedUnitTestException("Wrong number of nodes");
	if (mnet->get_edges(new_layer,new_layer).size() != 10) throw FailedUnitTestException("Wrong number of edges");
	std::cout << "done!" << std::endl;


	test_end("flattening");

}


