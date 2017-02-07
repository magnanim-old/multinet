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
#include "randomwalks.h"
#include "exceptions.h"
#include "measures.h"
#include "utils.h"
#include "io.h"

using namespace mlnet;

void test_randomwalks() {
	//log("TESTING Random Walks");

	//log("Reading network toy from file...",false);
	MLNetworkSharedPtr mnet3 = read_multilayer("test/toy.mpx","toy",',');
	//log("done! ",false);
	//log(mnet3->to_string());

	std::vector<double> p1({.4,.2,.2,.2});
	std::vector<double> p2({.2,.4,.2,.2});
	std::vector<double> p3({.2,.2,.4,.2});
	std::vector<double> p4({.2,.2,.2,.4});
	matrix<double> transitions({p1,p2,p3,p4});
	std::unordered_map<ActorSharedPtr, int > occ = occupation(mnet3,.2,transitions,10000);

	//for (const auto &p : occ) {
		//log(p.first->name + ": " + to_string(p.second));
	//}

	//log("Testing ...",false);
	//NodeSharedPtr n0 = mnet3->get_node(mnet3->get_actor("U0"),mnet3->get_layer("l1"));
	//NodeSharedPtr n1 = mnet3->get_node(mnet3->get_actor("U1"),mnet3->get_layer("l1"));
	//distance d(mnet3);
	//log(to_string(p.length()));
	//EdgeSharedPtr e = mnet3->get_edge(n0,n1);
	//p.step(e);
	//log(to_string(p.length()));
	//log("done!");




	//log("TEST SUCCESSFULLY COMPLETED (IO)");
}


