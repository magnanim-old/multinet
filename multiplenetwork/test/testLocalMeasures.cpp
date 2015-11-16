/*
 * testNetwork.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "test.h"
#include "multiplenetwork.h"
#include <iostream>
#include <string>

using namespace mlnet;

void testLocalMeasures() {

	//#ifndef TEST_IO
	//throw FailedUnitTestException("Local measure testing requires IO testing");
	//#endif

	log("TESTING measures (single actor)");
	log("Reading the multilayer network...",false);
	MLNetworkSharedPtr mnet = read_multilayer("test/io2.mpx","mlnet 2",',');
	LayerSharedPtr l1 = mnet->get_layer("l1");
	LayerSharedPtr l2 = mnet->get_layer("l2");
	ActorSharedPtr u1 = mnet->get_actor("U1");
	ActorSharedPtr u3 = mnet->get_actor("U3");
	std::unordered_set<LayerSharedPtr> nets = {l1,l2};
	log("done!");

	//print(mnet);

	log("Testing degree...",false);
	if (degree(mnet,u1,l1,INOUT) != 3) throw FailedUnitTestException("Wrong degree, actor U1, layer l1: " + to_string(degree(mnet,u1,l1,INOUT)));
	if (degree(mnet,u1,nets,INOUT) != 5) throw FailedUnitTestException("Wrong degree, both layers: " + to_string(degree(mnet,u1,nets,INOUT)));
	log("done!");

	log("Testing degree mean/deviation...",false);
	if (degree_mean(mnet,u1,nets,INOUT) != 2.5) throw FailedUnitTestException("Wrong mean degree, actor U1: " + to_string(degree_mean(mnet,u1,nets,INOUT)));
	if (degree_deviation(mnet,u1,nets,INOUT) != .5) throw FailedUnitTestException("Wrong degree deviation, actor U1: " + to_string(degree_deviation(mnet,u1,nets,INOUT)));
	log("done!");

	log("Testing neighborhood...",false);
	if (neighbors(mnet,u1,l1,INOUT).size() != 3) throw FailedUnitTestException("Wrong neighborhood, layer l1: " + to_string(neighbors(mnet,u1,l1,INOUT).size()));
	if (neighbors(mnet,u3,l1,IN).size() != 1) throw FailedUnitTestException("Wrong in_neighborhood, layer l1: " + to_string(neighbors(mnet,u3,l1,IN).size()));
	if (neighbors(mnet,u3,l1,OUT).size() != 1) throw FailedUnitTestException("Wrong out_neighborhood, layer l1: " + to_string(neighbors(mnet,u3,l1,OUT).size()));
	if (neighbors(mnet,u3,l2,IN).size() != 1) throw FailedUnitTestException("Wrong in_neighborhood, layer l2: " + to_string(neighbors(mnet,u3,l2,IN).size()));
	if (neighbors(mnet,u3,l2,OUT).size() != 1) throw FailedUnitTestException("Wrong out_neighborhood, layer l2: " + to_string(neighbors(mnet,u3,l2,OUT).size()));
	if (neighbors(mnet,u1,nets,INOUT).size() != 4) throw FailedUnitTestException("Wrong neighborhood, both layers: " + to_string(neighbors(mnet,u1,nets,INOUT).size()));
	log("done!");
	log("Testing exclusive neighborhood...",false);
	if (xneighbors(mnet,u1,l1,INOUT).size() != 2) throw FailedUnitTestException("Wrong exclusive neighborhood, layer l1: " + to_string(xneighbors(mnet,u1,l1,INOUT).size()));
	if (xneighbors(mnet,u1,nets,INOUT).size() != 4) throw FailedUnitTestException("Wrong exclusive neighborhood, both layers: " + to_string(xneighbors(mnet,u1,nets,INOUT).size()));
	log("done!");

	log("Testing layer relevance...",false);
	if (relevance(mnet,u1,l1,INOUT) != 3.0/4.0) throw FailedUnitTestException("Wrong relevance, layer l1: " + to_string(relevance(mnet,u1,l1,INOUT)));
	if (relevance(mnet,u1,nets,INOUT) != 1) throw FailedUnitTestException("Wrong relevance, both layers: " + to_string(relevance(mnet,u1,nets,INOUT)));
	log("done!");
	log("Testing exclusive layer relevance...",false);
	if (xrelevance(mnet,u1,l1,INOUT) != 2.0/4.0) throw FailedUnitTestException("Wrong exclusive relevance, layer l1: " + to_string(xrelevance(mnet,u1,l1,INOUT)));
	if (xrelevance(mnet,u1,nets,INOUT) != 1) throw FailedUnitTestException("Wrong exclusive relevance, both layers: " + to_string(xrelevance(mnet,u1,nets,INOUT)));
	log("done!");

	log("Testing jaccard similarity...",false);
	MLNetworkSharedPtr mnet1 = read_multilayer("test/io1.mpx","mlnet 1",',');
	//std::cout << network_jaccard_similarity(und_net,nets) << std::endl;
	if (jaccard_similarity(mnet,nets) != 1.0/10.0) throw FailedUnitTestException("Wrong layer similarity");
	/*MultiplexNetwork aucs = read_multiplex("data/aucs.mpx");
	std::set<std::string> aucsnets;
	aucsnets.insert("lunch");
	aucsnets.insert("facebook");
	std::cout << network_jaccard_similarity(aucs,aucsnets) << std::endl;
	*/
	log("done!");

	log("TEST SUCCESSFULLY COMPLETED (single actor)");



	/*
	log("Computing Pareto distance between all pairs of vertexes...",false);
	// The result is stored in the variable paths, where for each target vertex (from source U0) we obtain a set of shortest paths
	std::map<vertex_id,std::set<Path> > paths;
	pareto_distance_all_paths(mnet, mnet->getGlobalIdentity("U0"), paths);
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
	if (edge_betweenness[global_edge_id(0,1,mnet->getNetwork(0).isDirected(),0)]!=4) throw FailedUnitTestException("Wrong betweenness for edge U0-U1 on network 1");
	if (edge_betweenness[global_edge_id(3,4,mnet->getNetwork(1).isDirected(),1)]!=1) throw FailedUnitTestException("Wrong betweenness for edge U3-U4 on network 2");
	if (edge_betweenness[global_edge_id(1,3,mnet->getNetwork(0).isDirected(),0)]!=3) throw FailedUnitTestException("Wrong betweenness for edge U1-U3 on network 1");
	log("done!");

	log("TEST SUCCESSFULLY COMPLETED (distance and betweenness on undirected multiple networks)");
	*/
}


