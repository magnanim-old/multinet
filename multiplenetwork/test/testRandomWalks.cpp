/*
 * testNetwork.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#import <set>
#import "test.h"
#import "datastructures.h"
#import "exceptions.h"
#import "utils.h"
#import "io.h"

using namespace mlnet;

void testRandomWalks() {
	log("TESTING Random Walks");

	log("Reading network io3 from file...",false);
	MLNetworkSharedPtr mnet3 = read_multilayer("test/io3.mpx","mlnet 3",',');
	log("done! ",false);
	log(mnet3->to_string());

	//log("Testing ...",false);
	NodeSharedPtr n0 = mnet3->get_node(mnet3->get_actor("U0"),mnet3->get_layer("l1"));
	NodeSharedPtr n1 = mnet3->get_node(mnet3->get_actor("U1"),mnet3->get_layer("l1"));
	Path p(mnet3,n0);
	log(to_string(p.length()));
	EdgeSharedPtr e = mnet3->get_edge(n0,n1);
	p.step(e);
	log(to_string(p.length()));
	//log("done!");

	int stop_condition = 10;

	Path rw(mnet3,n0);
	while (stop_condition--) {
		 	if (random_utils::test(.8)) {
				SortedSet<NodeSharedPtr> neigh = mnet3->neighbors(rw.end(),OUT);
				log("Same layer - n: " + to_string(neigh.size()));
				if (neigh.size()==0)
					break;
				long rand = random_utils::getRandomInt(neigh.size());
				log("-R-> " + to_string(rand));
				NodeSharedPtr next = neigh.get_at_index(rand);
				log("--> " + next->to_string());
				rw.step(mnet3->get_edge(rw.end(),next));
				log("AH");
			}
			else {
				log("Actor change");
				SortedSet<NodeSharedPtr> nodes = mnet3->get_nodes(rw.end()->actor);
				if (nodes.size()==0)
					break;
				long rand = random_utils::getRandomInt(nodes.size());
				NodeSharedPtr next = nodes.get_at_index(rand);
				log("--> " + next->to_string());
			}
	}

	log("TEST SUCCESSFULLY COMPLETED (IO)");
}


