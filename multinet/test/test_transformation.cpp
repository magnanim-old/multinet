/*
 * testNetwork.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include <set>
#include "test.h"
#include "datastructures.h"
#include "exceptions.h"
#include "utils.h"
#include "io.h"

using namespace mlnet;

void test_transformation() {

	test_begin("Transformation");

	std::cout << "Creating ML network...";
	MLNetworkSharedPtr mnet = MLNetwork::create("test");
	ActorSharedPtr a1 = mnet->add_actor("a1");
	ActorSharedPtr a2 = mnet->add_actor("a2");
	ActorSharedPtr a3 = mnet->add_actor("a3");
	ActorSharedPtr a4 = mnet->add_actor("a4");
	ActorSharedPtr a5 = mnet->add_actor("a5");
	ActorSharedPtr a6 = mnet->add_actor("a6");
	ActorSharedPtr i1 = mnet->add_actor("I1");
	ActorSharedPtr i2 = mnet->add_actor("I2");
	ActorSharedPtr i3 = mnet->add_actor("I3");
	LayerSharedPtr L1 = mnet->add_layer("People 1",UNDIRECTED);
	LayerSharedPtr L2 = mnet->add_layer("People 2",UNDIRECTED);
	hash_set<LayerSharedPtr> people({L1, L2});
	LayerSharedPtr L3 = mnet->add_layer("Institutions",UNDIRECTED);
	NodeSharedPtr a1l1 = mnet->add_node(a1,L1);
	NodeSharedPtr a2l1 = mnet->add_node(a2,L1);
	NodeSharedPtr a3l1 = mnet->add_node(a3,L1);
	NodeSharedPtr a4l1 = mnet->add_node(a4,L1);
	NodeSharedPtr a5l1 = mnet->add_node(a5,L1);
	NodeSharedPtr a1l2 = mnet->add_node(a1,L2);
	NodeSharedPtr a2l2 = mnet->add_node(a2,L2);
	NodeSharedPtr a3l2 = mnet->add_node(a3,L2);
	NodeSharedPtr a4l2 = mnet->add_node(a4,L2);
	NodeSharedPtr a5l2 = mnet->add_node(a5,L2);
	NodeSharedPtr i1l3 = mnet->add_node(i1,L3);
	NodeSharedPtr i2l3 = mnet->add_node(i2,L3);
	NodeSharedPtr i3l3 = mnet->add_node(i3,L3);
	mnet->add_edge(a1l1,a2l1);
	mnet->add_edge(a1l1,a3l1);
	mnet->add_edge(a1l2,a2l2);
	mnet->add_edge(a3l2,a4l2);
	mnet->add_edge(a1l2,i1l3);
	mnet->add_edge(a2l2,i1l3);
	mnet->add_edge(a2l2,i2l3);
	mnet->add_edge(a3l2,i2l3);
	mnet->add_edge(a4l2,i2l3);
	mnet->add_edge(a5l2,i3l3);
	std::cout << "done! " << mnet->to_string() << std::endl;

	std::cout << "Flattening L1 and L2 (unweighted, only existing actors)...";
	LayerSharedPtr f1 = flatten_unweighted(mnet, "flattened1", people, false, false);
	if (mnet->is_directed(f1,f1)) throw FailedUnitTestException("Layer should be undirected");
	if (mnet->get_nodes(f1)->size() != 5) throw FailedUnitTestException("Wrong number of nodes");
	if (mnet->get_edges(f1,f1)->size() != 3) throw FailedUnitTestException("Wrong number of edges");
	std::cout << "done! " << mnet->to_string() << std::endl;

	std::cout << "Flattening L1 and L2 (unweighted, all actors)...";
	LayerSharedPtr f2 = flatten_unweighted(mnet, "flattened2", people, false, true);
	if (mnet->is_directed(f2,f2)) throw FailedUnitTestException("Layer should be undirected");
	if (mnet->get_nodes(f2)->size() != 9) throw FailedUnitTestException("Wrong number of nodes");
	if (mnet->get_edges(f2,f2)->size() != 3) throw FailedUnitTestException("Wrong number of edges");
	std::cout << "done! " << mnet->to_string() << std::endl;

	std::cout << "Flattening L1 and L2 (weighted, only existing actors)...";
	LayerSharedPtr f3 = flatten_weighted(mnet, "flattened3", people, false, false);
	if (mnet->is_directed(f3,f3)) throw FailedUnitTestException("Layer should be undirected");
	if (mnet->get_nodes(f3)->size() != 5) throw FailedUnitTestException("Wrong number of nodes");
	if (mnet->get_edges(f3,f3)->size() != 3) throw FailedUnitTestException("Wrong number of edges");
	NodeSharedPtr n1 = mnet->get_node(a1,f3);
	NodeSharedPtr n2 = mnet->get_node(a2,f3);
	NodeSharedPtr n3 = mnet->get_node(a3,f3);
	if (mnet->get_weight(n1,n2) != 2) throw FailedUnitTestException("Wrong weight, expected 2");
	if (mnet->get_weight(n1,n3) != 1) throw FailedUnitTestException("Wrong weight, expected 1");
	std::cout << "done! " << mnet->to_string() << std::endl;

	std::cout << "Projecting L3 into L2...";
	LayerSharedPtr p1 = project_unweighted(mnet, "projection", L2, L3);
	if (mnet->is_directed(p1,p1)) throw FailedUnitTestException("Layer should be undirected");
	if (mnet->get_nodes(p1)->size() != 5) throw FailedUnitTestException("Wrong number of nodes");
	if (mnet->get_edges(p1,p1)->size() != 4) throw FailedUnitTestException("Wrong number of edges");
	std::cout << "done! " << mnet->to_string() << std::endl;

	test_end("Transformation");
}


