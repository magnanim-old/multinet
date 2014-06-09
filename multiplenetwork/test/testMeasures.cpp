/*
 * testNetwork.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#import "test.h"
#import "multiplenetwork.h"
#include <iostream>
#include <string>

void testMeasures() {

	// We need to read the network from a file: testIO() must have been passed
	log("TESTING measures");
	log("Reading the network...",false);
	// Creating an empty multiple network and initializing it
	MultiplexNetwork mnet;
	read_multiplex(mnet, "test/io2.mpx");
	log("done!");

	log("Testing degree...",false);
	std::set<std::string> nets;
	nets.insert("l1");
	nets.insert("l2");
	if (degree(mnet,"U1","l1") != 3) throw FailedUnitTestException("Wrong degree, network l1");
	if (degree(mnet,"U1",nets) != 5) throw FailedUnitTestException("Wrong degree, both networks");
	log("done!");
	log("Testing neighborhood...",false);
	if (neighbors(mnet,"U1","l1").size() != 3) throw FailedUnitTestException("Wrong neighborhood, network l1");
	if (neighbors(mnet,"U1",nets).size() != 4) throw FailedUnitTestException("Wrong neighborhood, both networks");
	log("done!");
	log("Testing exclusive neighborhood...",false);
	if (xneighbors(mnet,"U1","l1").size() != 2) throw FailedUnitTestException("Wrong exclusive neighborhood, network l1");
	if (xneighbors(mnet,"U1",nets).size() != 4) throw FailedUnitTestException("Wrong exclusive neighborhood, both networks");
	log("done!");
	log("Testing network relevance...",false);
	if (relevance(mnet,"U1","l1") != 3.0/4.0) throw FailedUnitTestException("Wrong network relevance, network l1");
	if (relevance(mnet,"U1",nets) != 1) throw FailedUnitTestException("Wrong network relevance, both networks");
	log("done!");
	log("Testing exclusive network relevance...",false);
	if (xrelevance(mnet,"U1","l1") != 2.0/4.0) throw FailedUnitTestException("Wrong exclusive network relevance, network l1");
	if (xrelevance(mnet,"U1",nets) != 1) throw FailedUnitTestException("Wrong exclusive network relevance, both networks");
	log("done!");

	log("TEST SUCCESSFULLY COMPLETED (node-based analysis measures)");

	/*
	log("Computing Pareto distance between all pairs of vertexes...",false);
	// The result is stored in the variable paths, where for each target vertex (from source U0) we obtain a set of shortest paths
	std::map<vertex_id,std::set<Path> > paths;
	pareto_distance_all_paths(mnet, mnet.getGlobalIdentity("U0"), paths);
	log("done!");

	log("Testing sample values (U0->U3: 2 shortest paths expected)...",false);
	if (paths[3].size()!=2) throw FailedUnitTestException("Expected 2 paths, found " + std::to_string(paths[3].size()));
	Path p1 = *paths[3].begin();
	Path p2 = *++paths[3].begin();
	if (p1.length()!=2) throw FailedUnitTestException("Wrong length: path 1, " + std::to_string(p1.length()));
	if (p2.length()!=2) throw FailedUnitTestException("Wrong length: path 2, " + std::to_string(p2.length()));
	if (p1.getNumEdgesOnNetwork(0)!=2) throw FailedUnitTestException("Wrong number of edges: path 1 on network l1");
	if (p1.getNumEdgesOnNetwork(1)!=0) throw FailedUnitTestException("Wrong number of edges: path 1 on network l2");
	if (p2.getNumEdgesOnNetwork(0)!=1) throw FailedUnitTestException("Wrong number of edges: path 2 on network l1");
	if (p2.getNumEdgesOnNetwork(1)!=1) throw FailedUnitTestException("Wrong number of edges: path 2 on network l2");
	log("done!");

	log("Computing Pareto betweenness for all vertexes...",false);
	std::map<vertex_id, long> vertex_betweenness;
	pareto_betweenness(mnet, vertex_betweenness);
	log("done!");

	log("Testing sample values (U1=9, U3=0)...",false);
	if (vertex_betweenness[1]!=9) throw FailedUnitTestException("Wrong betweenness for node U1");
	if (vertex_betweenness[3]!=0) throw FailedUnitTestException("Wrong betweenness for node U3");
	log("done!");


	log("Computing Pareto betweenness for all edges...",false);
	std::map<global_edge_id, long> edge_betweenness;
	pareto_edge_betweenness(mnet, edge_betweenness);
	log("done!");

	log("Testing sample values (network 1: U0-U1=4, U1-U3=3, network 2: U3-U4=1)...",false);
	if (edge_betweenness[global_edge_id(0,1,mnet.getNetwork(0).isDirected(),0)]!=4) throw FailedUnitTestException("Wrong betweenness for edge U0-U1 on network 1");
	if (edge_betweenness[global_edge_id(3,4,mnet.getNetwork(1).isDirected(),1)]!=1) throw FailedUnitTestException("Wrong betweenness for edge U3-U4 on network 2");
	if (edge_betweenness[global_edge_id(1,3,mnet.getNetwork(0).isDirected(),0)]!=3) throw FailedUnitTestException("Wrong betweenness for edge U1-U3 on network 1");
	log("done!");

	log("TEST SUCCESSFULLY COMPLETED (distance and betweenness on undirected multiple networks)");
	*/
}


