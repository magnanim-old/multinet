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

void testIO() {
	log("TESTING IO (MultiplexNetwork)");
	log("Reading network io1 from file...",false);
	MultiplexNetwork mnet1;
	read_multiplex(mnet1, "test/io1.mpx");
	log("done!");
	print(mnet1);
	std::set<network_id> nets1;
	mnet1.getNetworks(nets1);
	for (network_id nid: nets1) {
		print(mnet1.getNetwork(nid));
	}

	log("Reading network io2 from file...",false);
	MultiplexNetwork mnet2;
	read_multiplex(mnet2, "test/io2.mpx");
	log("done!");
	print(mnet2);
	std::set<network_id> nets2;
	mnet2.getNetworks(nets2);
	for (network_id nid: nets2) {
		print(mnet2.getNetwork(nid));
	}

	log("Reading network io3 from file...",false);
	MultiplexNetwork mnet3;
	read_multiplex(mnet3, "test/io3.mpx");
	log("done!");
	print(mnet3);
	std::set<network_id> nets3;
	mnet3.getNetworks(nets3);
	for (network_id nid: nets3) {
		print(mnet3.getNetwork(nid));
	}

	log("Testing attribute values...",false);
	if (mnet3.getNetwork("l1").getStringVertexAttribute("U0","Color")!="Blue") throw FailedUnitTestException("vertex string attribute not correctly read from file");
	if (mnet3.getNetwork("l1").getNumericVertexAttribute("U0","Age")!=34) throw FailedUnitTestException("vertex numeric attribute not correctly read from file");
	if (mnet3.getNetwork("l2").getEdgeWeight("U1","U3")!=3) throw FailedUnitTestException("edge weight not correctly read from file");
	if (mnet3.getNetwork("l2").getNumericEdgeAttribute("U1","U3","Stars")!=4) throw FailedUnitTestException("edge numeric attribute not correctly read from file");
	log("done!");

	log("TEST SUCCESSFULLY COMPLETED (IO - MultiplexNetwork, edgelist)");
}


