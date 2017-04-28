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
#include "../include/multinet.h"

using namespace mlnet;



void test_community() {

	test_begin("community data structures");

	MLNetworkSharedPtr mnet = read_multilayer("aucs.mpx","aucs",',');

	ActorSharedPtr a = mnet->get_actor("U54");
	LayerSharedPtr l1 = mnet->get_layer("lunch");
	LayerSharedPtr l2 = mnet->get_layer("leisure");	

	CommunitySharedPtr c1 = community::create();
	c1->add_node(mnet->get_node(a,l1));
	c1->add_node(mnet->get_node(a,l2));

    CommunitySharedPtr c2 = community::create();
	c2->add_node(mnet->get_node(a,l1));

	CommunitiesSharedPtr com = communities::create();
	com->add_community(c1);
	com->add_community(c2);

	std::cout << com->to_string() << std::endl;

	test_end("community data structures");
}
