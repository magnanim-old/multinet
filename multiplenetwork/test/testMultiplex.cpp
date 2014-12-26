/*
 * testMultipleNetwork.cpp
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: 0.0.1
 */

#import "test.h"
#import "datastructures.h"
#import "exceptions.h"
#import "utils.h"

void testMultiplex() {
	log("******************************************");
	log("TESTING Multiplex");
	log("REQUIRES Network class having been tested");

	log("Creating an empty multiple network...",false);
	MultiplexNetwork mnet;
	log("done!");

	log("Adding seven global vertexes with different methods...",false);
	identity gv0 = mnet.addGlobalIdentity();
	identity gv1 = mnet.addGlobalIdentity();
	identity gv2 = mnet.addGlobalIdentity();
	identity gv3 = mnet.addGlobalIdentity();
	mnet.addGlobalIdentities(3);
	if (mnet.getNumGlobalIdentities()!=7) throw FailedUnitTestException("Wrong number of global vertexes");
	log("done!");

	log("Creating and adding three anonymous undirected and directed networks...",false);
	// Network 1
	Network net1(false,false,false);
	vertex_id n1v0 = net1.addVertex();
	vertex_id n1v1 = net1.addVertex();
	vertex_id n1v2 = net1.addVertex();
	// Network 2
	Network net2(false,true,false);
	vertex_id n2v0 = net2.addVertex();
	vertex_id n2v1 = net2.addVertex();
	vertex_id n2v2 = net2.addVertex();
	// Network 3
	Network net3(false,true,false);
	vertex_id n3v0 = net3.addVertex();
	vertex_id n3v1 = net3.addVertex();
	vertex_id n3v2 = net3.addVertex();

	network_id n1 = mnet.addNetwork(net1);
	network_id n2 = mnet.addNetwork(net2);
	network_id n3 = mnet.addNetwork(net3);
	if (mnet.getNumNetworks()!=3) throw FailedUnitTestException("Wrong number of networks");
	log("done!");

	log("Specifying vertex mappings...",false);
	// To network 1
	mnet.mapIdentity(gv0,n1v0,n1);
	mnet.mapIdentity(gv1,n1v1,n1);
	mnet.mapIdentity(gv2,n1v2,n1);
	// To network 2
	mnet.mapIdentity(gv0,n2v0,n2);
	mnet.mapIdentity(gv1,n2v1,n2);
	mnet.mapIdentity(gv3,n2v2,n2);
	// To network 3
	mnet.mapIdentity(gv0,n3v0,n3);
	mnet.mapIdentity(gv2,n3v1,n3);
	mnet.mapIdentity(gv3,n3v2,n3);
	if (mnet.getGlobalIdentity(n3v2,n3)!=gv3) throw FailedUnitTestException("Wrong mapping between global and local vertex");
	if (mnet.getVertexId(gv3,n2)!=n2v2) throw FailedUnitTestException("Wrong mapping between global and local vertex");
	log("done!");

	/* behavior to be decided and tested
	log("Adding duplicate vertex mapping...",false);
	try {
		mnet.map(gv3,n3v2,n3);
		// should not arrive here
		throw FailedUnitTestException("Duplicate value non caught");
	}
	catch (DuplicateElementException& ex) {
		log("correctly thrown Exception!");
	}
	*/

	log("Adding five edges...",false); // they can also be added directly to the single networks
	mnet.getNetwork(n1).addEdge(n1v0,n1v1);
	mnet.getNetwork(n2).addEdge(n2v0,n2v1);
	mnet.getNetwork(n2).addEdge(n2v1,n2v2);
	mnet.getNetwork(n3).addEdge(n3v0,n3v2);
	mnet.getNetwork(n3).addEdge(n3v1,n3v2);
	if (mnet.getNumEdges()!=5) throw FailedUnitTestException("Wrong number of global edges");
	log("done!");

	log("Mapping to a non existing network...",false);
	try {
		mnet.mapIdentity(gv3,n2v2,n3+1);
		// should not arrive here
		throw FailedUnitTestException("Non existing network non caught");
	}
	catch (ElementNotFoundException& ex) {
		log("[FAIL] done!");
	}
	log("Mapping between non existing vertexes...",false);
	try {
		mnet.mapIdentity(gv3,n2v2+1,n2);
		// should not arrive here
		throw FailedUnitTestException("Non existing vertex non caught");
	}
	catch (ElementNotFoundException& ex) {
		log("[FAIL] done!");
	}

	log("TEST SUCCESSFULLY COMPLETED (Multiplex class - numeric identifiers)");

	log("Printing final multiplex network information:");
	print(mnet);

}


