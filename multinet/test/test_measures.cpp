/*
 * testNetwork.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "test.h"
#include <iostream>
#include <string>
#include "../include/multinet.h"

using namespace mlnet;

void test_measures() {


	test_begin("Actor measures");
	std::cout << "Reading the multilayer network...";
	MLNetworkSharedPtr mnet = read_multilayer("test/io2.mpx","mlnet 2",',');
	std::cout << "done!" << std::endl;
	LayerSharedPtr l1 = mnet->get_layer("l1");
	LayerSharedPtr l2 = mnet->get_layer("l2");
	ActorSharedPtr u1 = mnet->get_actor("U1");
	ActorSharedPtr u3 = mnet->get_actor("U3");
	std::unordered_set<LayerSharedPtr> layers = {l1,l2};

	std::cout << "Testing degree...";
	if (degree(mnet,u1,l1,INOUT) != 3) throw FailedUnitTestException("Wrong degree, actor U1, layer l1: " + to_string(degree(mnet,u1,l1,INOUT)));
	if (degree(mnet,u1,layers,INOUT) != 5) throw FailedUnitTestException("Wrong degree, both layers: " + to_string(degree(mnet,u1,layers,INOUT)));
	std::cout << "done!" << std::endl;

	std::cout << "Testing degree mean and degree deviation...";
	if (degree_mean(mnet,u1,layers,INOUT) != 2.5) throw FailedUnitTestException("Wrong mean degree, actor U1: " + to_string(degree_mean(mnet,u1,layers,INOUT)));
	if (degree_deviation(mnet,u1,layers,INOUT) != .5) throw FailedUnitTestException("Wrong degree deviation, actor U1: " + to_string(degree_deviation(mnet,u1,layers,INOUT)));
	std::cout << "done!" << std::endl;

	if (neighbors(mnet,u1,l1,INOUT)->size() != 3) throw FailedUnitTestException("Wrong neighborhood, layer l1: " + to_string(neighbors(mnet,u1,l1,INOUT)->size()));
	if (neighbors(mnet,u3,l1,IN)->size() != 1) throw FailedUnitTestException("Wrong in_neighborhood, layer l1: " + to_string(neighbors(mnet,u3,l1,IN)->size()));
	if (neighbors(mnet,u3,l1,OUT)->size() != 1) throw FailedUnitTestException("Wrong out_neighborhood, layer l1: " + to_string(neighbors(mnet,u3,l1,OUT)->size()));
	if (neighbors(mnet,u3,l2,IN)->size() != 1) throw FailedUnitTestException("Wrong in_neighborhood, layer l2: " + to_string(neighbors(mnet,u3,l2,IN)->size()));
	if (neighbors(mnet,u3,l2,OUT)->size() != 1) throw FailedUnitTestException("Wrong out_neighborhood, layer l2: " + to_string(neighbors(mnet,u3,l2,OUT)->size()));
	if (neighbors(mnet,u1,layers,INOUT)->size() != 4) throw FailedUnitTestException("Wrong neighborhood, both layers: " + to_string(neighbors(mnet,u1,layers,INOUT)->size()));
	std::cout << "done!" << std::endl;

	std::cout << "Testing exclusive neighborhood...";
	if (xneighbors(mnet,u1,l1,INOUT)->size() != 2) throw FailedUnitTestException("Wrong exclusive neighborhood, layer l1: " + to_string(xneighbors(mnet,u1,l1,INOUT)->size()));
	if (xneighbors(mnet,u1,layers,INOUT)->size() != 4) throw FailedUnitTestException("Wrong exclusive neighborhood, both layers: " + to_string(xneighbors(mnet,u1,layers,INOUT)->size()));
	std::cout << "done!" << std::endl;

	std::cout << "Testing connective redundancy...";
	if (connective_redundancy(mnet,u1,layers,INOUT) != (1-4/5.0)) throw FailedUnitTestException("Wrong connective redundancy: " + to_string(connective_redundancy(mnet,u1,layers,INOUT)));
	std::cout << "done!" << std::endl;

	std::cout << "Testing layer relevance...";
	if (relevance(mnet,u1,l1,INOUT) != 3.0/4.0) throw FailedUnitTestException("Wrong relevance, layer l1: " + to_string(relevance(mnet,u1,l1,INOUT)));
	if (relevance(mnet,u1,layers,INOUT) != 1) throw FailedUnitTestException("Wrong relevance, both layers: " + to_string(relevance(mnet,u1,layers,INOUT)));
	std::cout << "done!" << std::endl;

	std::cout << "Testing exclusive layer relevance...";
	if (xrelevance(mnet,u1,l1,INOUT) != 2.0/4.0) throw FailedUnitTestException("Wrong exclusive relevance, layer l1: " + to_string(xrelevance(mnet,u1,l1,INOUT)));
	if (xrelevance(mnet,u1,layers,INOUT) != 1) throw FailedUnitTestException("Wrong exclusive relevance, both layers: " + to_string(xrelevance(mnet,u1,layers,INOUT)));
	std::cout << "done!" << std::endl;


	MLNetworkSharedPtr mnet5 = read_multilayer("test/io5.mpx","mlnet 5",',');
	l1 = mnet5->get_layer("l1");
	l2 = mnet5->get_layer("l2");
	double sim;
	property_matrix<ActorSharedPtr,LayerSharedPtr,bool> AEP = actor_existence_property_matrix(mnet5);
	property_matrix<dyad,LayerSharedPtr,bool> EEP = edge_existence_property_matrix(mnet5);
	property_matrix<triad,LayerSharedPtr,bool> TEP = triangle_existence_property_matrix(mnet5);
	std::cout << "Testing actor jaccard similarity...";
	sim = jaccard(AEP,l1,l2);
	std::cout << "[" << sim << "] ";
	if (sim != 5.0/6.0) throw FailedUnitTestException("Wrong actor jaccard layer similarity");
	std::cout << "done!" << std::endl;
	std::cout << "Testing edge jaccard similarity...";
	sim = jaccard(EEP,l1,l2);
	std::cout << "[" << sim << "] ";
	if (sim != 4.0/8.0) throw FailedUnitTestException("Wrong edge jaccard layer similarity");
	std::cout << "done!" << std::endl;
	std::cout << "Testing triangle jaccard similarity...";
	sim = jaccard(TEP,l1,l2);
	std::cout << "[" << sim << "] ";
	if (sim != 1.0/2.0) throw FailedUnitTestException("Wrong triangle jaccard layer similarity");
	std::cout << "done!" << std::endl;
	std::cout << "Testing actor coverage similarity...";
	sim = coverage(AEP,l1,l2);
	std::cout << "[" << sim << "] ";
	if (sim != 5.0/6.0) throw FailedUnitTestException("Wrong actor coverage layer similarity");
	std::cout << "done!" << std::endl;
	std::cout << "Testing edge coverage similarity...";
	sim = coverage(EEP,l1,l2);
	std::cout << "[" << sim << "] ";
	if (sim != 4.0/6.0) throw FailedUnitTestException("Wrong edge coverage layer similarity");
	std::cout << "done!" << std::endl;
	std::cout << "Testing triangle coverage similarity...";
	sim = coverage(TEP,l1,l2);
	std::cout << "[" << sim << "] ";
	if (sim != 1.0/1.0) throw FailedUnitTestException("Wrong triangle coverage layer similarity");
	std::cout << "done!" << std::endl;
	std::cout << "Testing actor simple matching similarity...";
	sim = simple_matching(AEP,l1,l2);
	std::cout << "[" << sim << "] ";
	if (sim != 5.0/6.0) throw FailedUnitTestException("Wrong actor simple matching layer similarity");
	std::cout << "done!" << std::endl;
	std::cout << "Testing edge simple matching similarity...";
	sim = simple_matching(EEP,l1,l2);
	std::cout << "[" << sim << "] ";
	if (sim != 11.0/15.0) throw FailedUnitTestException("Wrong edge simple matching layer similarity");
	std::cout << "done!" << std::endl;
	std::cout << "Testing triangle simple matching similarity...";
	sim = simple_matching(TEP,l1,l2);
	std::cout << "[" << sim << "] ";
	if (sim != 19.0/20.0) throw FailedUnitTestException("Wrong triangle simple matching layer similarity");
	std::cout << "done!" << std::endl;
	std::cout << "Testing actor kulczynski2 similarity...";
	sim = kulczynski2(AEP,l1,l2);
	std::cout << "[" << sim << "] ";
	if (std::abs(sim-55.0/60.0) > 0.0001) throw FailedUnitTestException("Wrong actor kulczynski2 layer similarity");
	std::cout << "done!" << std::endl;
	std::cout << "Testing edge kulczynski2 similarity...";
	sim = kulczynski2(EEP,l1,l2);
	std::cout << "[" << sim << "] ";
	if (std::abs(sim-4.0/6.0) > 0.0001) throw FailedUnitTestException("Wrong edge kulczynski2 layer similarity");
	std::cout << "done!" << std::endl;
	std::cout << "Testing triangle kulczynski2 similarity...";
	sim = kulczynski2(TEP,l1,l2);
	std::cout << "[" << sim << "] ";
	if (sim != 3.0/4.0) throw FailedUnitTestException("Wrong triangle kulczynski2 layer similarity");
	std::cout << "done!" << std::endl;
	property_matrix<ActorSharedPtr,LayerSharedPtr,double> DegP = actor_degree_property_matrix(mnet5,INOUT);
	std::cout << "Testing linear (pearson) degree correlation...";
	sim = pearson(DegP,l1,l2);
	std::cout << "[" << sim << "] ";
	if (std::abs(sim-0.57735) > 0.0001) throw FailedUnitTestException("Wrong linear (pearson) degree correlation");
	std::cout << "done!" << std::endl;
	std::cout << "Testing rank (spearman) degree correlation...";
	DegP.rankify();
	sim = pearson(DegP,l1,l2);
	std::cout << "[" << sim << "] ";
	if (std::abs(sim-0.688617) > 0.0001) throw FailedUnitTestException("Wrong linear (pearson) degree correlation");
	std::cout << "done!" << std::endl;


	std::cout << "Computing multilayer distance between all pairs of vertexes...";
	// The result is stored in the variable paths, where for each target vertex (from source U0) we obtain a set of shortest paths
	hash_map<ActorSharedPtr,std::set<path_length> > dists = pareto_distance(mnet, mnet->get_actor("U0"));
	std::cout << "done!" << std::endl;

	for (auto p: dists) {
		for (auto dist: p.second) {
			std::cout << "- dist from U0 to " <<  p.first->name << ": " << dist.to_string() << std::endl;
		}
	}

	// TODO

	std::cout << "Testing sample values (U0->U3: 2 shortest paths expected)...";
	if (dists[mnet->get_actor("U3")].size()!=2) throw FailedUnitTestException("Expected 2 distances, found " + to_string(dists[mnet->get_actor("U3")].size()));
	path_length p1 = *dists[mnet->get_actor("U3")].begin();
	path_length p2 = *++dists[mnet->get_actor("U3")].begin();
	if (p1.length()!=2) throw FailedUnitTestException("Wrong length: distance 1, " + to_string(p1.length()));
	if (p2.length()!=2) throw FailedUnitTestException("Wrong length: distance 2, " + to_string(p2.length()));
	std::cout << "done!" << std::endl;

	test_end("Actor measures");

}


