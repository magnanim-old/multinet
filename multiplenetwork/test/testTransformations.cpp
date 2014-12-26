/*
 * testNetwork.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#import "test.h"
#import "multiplenetwork.h"
#import <iostream>

void testTransformations() {
	log("TESTING transformations");
	log("Reading multiple network from file...",false);
	// Creating an empty multiple network and initializing it
	MultiplexNetwork mnet = read_multiplex("test/io2.mpx");
	log("done!");

	log("Testing weighted flattening...",false);
	std::set<std::string> nets;
	nets.insert("l1");
	nets.insert("l2");

	Network net = flatten_weighted(mnet,nets,false);

	std::cout << net.getEdgeWeight("U1","U2") << " " << net.getEdgeWeight("U1","U3") << std::endl;
	if (!net.isDirected()) throw FailedUnitTestException("Network should be directed");
	if (net.getNumVertexes() != 6) throw FailedUnitTestException("Wrong number of vertexes");
	if (net.getNumEdges() != 10) throw FailedUnitTestException("Wrong number of edges");
	if (net.getEdgeWeight("U1","U2") != 1) throw FailedUnitTestException("Wrong weight, expected 1");
	if (net.getEdgeWeight("U1","U3") != 2) throw FailedUnitTestException("Wrong weight, expected 2");
	log("done!");

}


