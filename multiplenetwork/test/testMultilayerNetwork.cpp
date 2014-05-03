/*
 * testMultilayerNetwork.cpp
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: 0.0.1
 */

#import "test.h"
#import "datastructures.h"
#import "exceptions.h"
#import "utils.h"

void testMultilayerNetwork() {
	log("******************************************");
	log("TESTING MultilayerNetwork");
	log("REQUIRES Network class having been tested");

	log("Creating an empty multiple network...",false);
	MultilayerNetwork mnet;
	log("done!");

	log("Creating and adding three anonymous undirected and directed networks...",false);
	// Network 1
	Network net1(false,false,false);
	vertex_id n1v0 = net1.addVertex();
	vertex_id n1v1 = net1.addVertex();
	/*vertex_id n1v2 = */net1.addVertex();
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

	// TODO Check named networks

	log("Adding five edges...",false); // they can also be added directly to the single networks
	mnet.getNetwork(n1).addEdge(n1v0,n1v1);
	mnet.getNetwork(n2).addEdge(n2v0,n2v1);
	mnet.getNetwork(n2).addEdge(n2v1,n2v2);
	mnet.getNetwork(n3).addEdge(n3v0,n3v2);
	mnet.getNetwork(n3).addEdge(n3v1,n3v2);
	if (mnet.getNumEdges()!=5) throw FailedUnitTestException("Wrong number of global edges");
	log("done!");

	// Iterating through global edges and vertexes
	std::set<global_vertex_id> vertexes;
	std::set<global_edge_id> edges;

	log("TEST SUCCESSFULLY COMPLETED (MultilayerNetwork class - numeric identifiers)");

	log("Printing final multiple network information:");
	print(mnet);

}


