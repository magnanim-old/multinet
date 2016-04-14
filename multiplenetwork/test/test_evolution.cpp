
#include "test.h"
#include "mlnetwork.h"
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <iostream>
#include <sstream>
#include <string>

using namespace mlnet;

void test_evolution() {
	{
	test_begin("[Evolution Test R1] N1: larger independent, N2: smaller independent and with basic random graph evolution model");
	// create multiplex network
	MLNetworkSharedPtr mnet = MLNetwork::create("synt1");
	LayerSharedPtr layer1 = mnet->add_layer("L1",true);
	LayerSharedPtr layer2 = mnet->add_layer("L2",true);
	// set evolution parameters
	long num_of_steps = 100;
	long num_of_actors = 1000;
	std::vector<double> pr_no_event = {0.0,0.3};
	std::vector<double> pr_internal_event = {1.0,1.0};
	matrix<double> dependency = {{0, 1}, {1, 0}};
	BAEvolutionModel ba(3,2);
	UniformEvolutionModel ra(70);
	std::vector<EvolutionModel*> evolution_model = {&ba, &ra};
	evolve(mnet,num_of_steps,num_of_actors,pr_no_event,pr_internal_event,dependency,evolution_model);
	//log(mnet->to_string());
	//log("Edge Jaccard similarity: " + to_string(jaccard_similarity(mnet,layer1,layer2)));
	//log("Assortativity: " + to_string(assortativity(mnet,layer1,layer2,OUT)));
	}

	{
	test_begin("[Evolution Test R2] N1: larger independent, N2: smaller dependent on N1, both with basic random graph evolution model");
	MLNetworkSharedPtr mnet = MLNetwork::create("synt1");
	LayerSharedPtr layer1 = mnet->add_layer("L1",true);
	LayerSharedPtr layer2 = mnet->add_layer("L2",true);
	// set evolution parameters
	long num_of_steps = 100;
	long num_of_actors = 1000;
	std::vector<double> pr_no_event = {0.0,0.3};
	std::vector<double> pr_internal_event = {1.0,0.5};
	matrix<double> dependency = {{0, 1}, {1, 0}};
	UniformEvolutionModel ra(30);
	std::vector<EvolutionModel*> evolution_model = {&ra, &ra};
	evolve(mnet,num_of_steps,num_of_actors,pr_no_event,pr_internal_event,dependency,evolution_model);
	//log(mnet->to_string());
	//log("Edge Jaccard similarity: " + to_string(jaccard_similarity(mnet,layer1,layer2)));
	//log("Assortativity: " + to_string(assortativity(mnet,layer1,layer2,OUT)));
	}
	/*
	{
	log("[Evolution Test 1] N1: larger independent, N2: smaller independent");
	// create multiplex network
	MultiplexNetwork mnet1;
	for (int i=0; i<1000; i++) mnet1.addGlobalName("U"+std::to_string(i));
	Network n1(true,false,false), n2(true,false,false);
	network_id n1_1 = mnet1.addNetwork("N1",n1);
	network_id n1_2 = mnet1.addNetwork("N2",n2);
	// set evolution parameters
	long num_of_steps = 100;
	std::vector<double> pr_no_event = {0.0,0.3};
	std::vector<double> pr_internal_event = {1.0,1.0};
	vector<vector<double> > dependency = {{0, 1}, {1, 0}};
	BAEvolutionModel ba(3,2);
	std::vector<EvolutionModel*> evolution_model = {&ba, &ba};
	evolve(mnet1,num_of_steps,pr_no_event,pr_internal_event,dependency,evolution_model);
	log("Size 1: " + std::to_string(mnet1.getNetwork(n1_1).getNumVertexes()) + " v, " + std::to_string(mnet1.getNetwork(n1_1).getNumEdges()) + " e");
	log("Size 2: " + std::to_string(mnet1.getNetwork(n1_2).getNumVertexes()) + " v, " + std::to_string(mnet1.getNetwork(n1_2).getNumEdges()) + " e");
	int common_vertexes = 0;
	for (vertex_id v: mnet1.getNetwork(n1_1).getVertexes()) {
		global_identity id = mnet1.getGlobalIdentity(v, n1_1);
		if (mnet1.containsVertex(id,n1_2))
			common_vertexes++;
	}
	log("Common vertexes: " + std::to_string(common_vertexes));
	log("Edge Jaccard similarity: " + std::to_string(network_jaccard_similarity(mnet1,n1_1,n1_2)));
	}
	{
	log("[Evolution Test 2] N1: larger independent, N2: smaller dependent on N1");
	// create multiplex network
	MultiplexNetwork mnet1;
	for (int i=0; i<1000; i++) mnet1.addGlobalName("U"+std::to_string(i));
	Network n1(true,false,false), n2(true,false,false);
	network_id n1_1 = mnet1.addNetwork("N1",n1);
	network_id n1_2 = mnet1.addNetwork("N2",n2);
	// set evolution parameters
	long num_of_steps = 100;
	std::vector<double> pr_no_event = {0.0,0.3};
	std::vector<double> pr_internal_event = {1.0,0.7};
	vector<vector<double> > dependency = {{0, 1}, {1, 0}};
	BAEvolutionModel ba(3,2);
	std::vector<EvolutionModel*> evolution_model = {&ba, &ba};
	evolve(mnet1,num_of_steps,pr_no_event,pr_internal_event,dependency,evolution_model);
	log("Size 1: " + std::to_string(mnet1.getNetwork(n1_1).getNumVertexes()) + " v, " + std::to_string(mnet1.getNetwork(n1_1).getNumEdges()) + " e");
	log("Size 2: " + std::to_string(mnet1.getNetwork(n1_2).getNumVertexes()) + " v, " + std::to_string(mnet1.getNetwork(n1_2).getNumEdges()) + " e");
	int common_vertexes = 0;
	for (vertex_id v: mnet1.getNetwork(n1_1).getVertexes()) {
		global_identity id = mnet1.getGlobalIdentity(v, n1_1);
		if (mnet1.containsVertex(id,n1_2))
			common_vertexes++;
	}
	log("Common vertexes: " + std::to_string(common_vertexes));
	log("Edge Jaccard similarity: " + std::to_string(network_jaccard_similarity(mnet1,n1_1,n1_2)));
	}
	{
	log("[Evolution Test 3] N1: larger independent, N2: smaller almost completely dependent on N1");
	// create multiplex network
	MultiplexNetwork mnet1;
	for (int i=0; i<1000; i++) mnet1.addGlobalName("U"+std::to_string(i));
	Network n1(true,false,false), n2(true,false,false);
	network_id n1_1 = mnet1.addNetwork("N1",n1);
	network_id n1_2 = mnet1.addNetwork("N2",n2);
	// set evolution parameters
	long num_of_steps = 100;
	std::vector<double> pr_no_event = {0.0,0.3};
	std::vector<double> pr_internal_event = {1.0,0.1};
	vector<vector<double> > dependency = {{0, 1}, {1, 0}};
	BAEvolutionModel ba(3,2);
	std::vector<EvolutionModel*> evolution_model = {&ba, &ba};
	evolve(mnet1,num_of_steps,pr_no_event,pr_internal_event,dependency,evolution_model);
	log("Size 1: " + std::to_string(mnet1.getNetwork(n1_1).getNumVertexes()) + " v, " + std::to_string(mnet1.getNetwork(n1_1).getNumEdges()) + " e");
	log("Size 2: " + std::to_string(mnet1.getNetwork(n1_2).getNumVertexes()) + " v, " + std::to_string(mnet1.getNetwork(n1_2).getNumEdges()) + " e");
	int common_vertexes = 0;
	for (vertex_id v: mnet1.getNetwork(n1_1).getVertexes()) {
		global_identity id = mnet1.getGlobalIdentity(v, n1_1);
		if (mnet1.containsVertex(id,n1_2))
			common_vertexes++;
	}
	log("Common vertexes: " + std::to_string(common_vertexes));
	log("Edge Jaccard similarity: " + std::to_string(network_jaccard_similarity(mnet1,n1_1,n1_2)));
	}
	{
	log("[Evolution Test 4] N1: larger independent, N2: smaller dependent more on N1 than on N3, N3: smaller almost independent");
	// create multiplex network
	MultiplexNetwork mnet1;
	for (int i=0; i<1000; i++) mnet1.addGlobalName("U"+std::to_string(i));
	Network n1(true,false,false), n2(true,false,false), n3(true,false,false);
	network_id n1_1 = mnet1.addNetwork("N1",n1);
	network_id n1_2 = mnet1.addNetwork("N2",n2);
	network_id n1_3 = mnet1.addNetwork("N3",n3);
	// set evolution parameters
	long num_of_steps = 100;
	std::vector<double> pr_no_event = {0.0,0.2,0.2};
	std::vector<double> pr_internal_event = {1.0,0.3,0.9};
	vector<vector<double> > dependency = {{0, 1, 1}, {1, 0, .3}, {1, 1, 0}};
	BAEvolutionModel ba(3,2);
	std::vector<EvolutionModel*> evolution_model = {&ba, &ba, &ba};
	evolve(mnet1,num_of_steps,pr_no_event,pr_internal_event,dependency,evolution_model);
	log("Size 1: " + std::to_string(mnet1.getNetwork(n1_1).getNumVertexes()) + " v, " + std::to_string(mnet1.getNetwork(n1_1).getNumEdges()) + " e");
	log("Size 2: " + std::to_string(mnet1.getNetwork(n1_2).getNumVertexes()) + " v, " + std::to_string(mnet1.getNetwork(n1_2).getNumEdges()) + " e");
	log("Size 3: " + std::to_string(mnet1.getNetwork(n1_3).getNumVertexes()) + " v, " + std::to_string(mnet1.getNetwork(n1_3).getNumEdges()) + " e");
	log("Edge Jaccard similarity (n1-n2): " + std::to_string(network_jaccard_similarity(mnet1,n1_1,n1_2)));
	log("Edge Jaccard similarity (n1-n3): " + std::to_string(network_jaccard_similarity(mnet1,n1_1,n1_3)));
	log("Edge Jaccard similarity (n2-n3): " + std::to_string(network_jaccard_similarity(mnet1,n1_3,n1_2)));
	}
	*/
	{
	test_begin("[Evolution Test 5] LARGER NETWORKS, COMMON ACTORS - N1: larger independent, N2: smaller dependent more on N1 than on N3, N3: smaller almost independent");
	// create multiplex network
	MLNetworkSharedPtr mnet = MLNetwork::create("synt1");
		LayerSharedPtr layer1 = mnet->add_layer("L1",true);
		LayerSharedPtr layer2 = mnet->add_layer("L2",true);
		LayerSharedPtr layer3 = mnet->add_layer("L3",true);
		// set evolution parameters
		long num_of_steps = 1000;
		long num_of_actors = 10000;
		std::vector<double> pr_no_event = {0.0,0.3,0.3};
		std::vector<double> pr_internal_event = {1,0,0.9};
		matrix<double> dependency = {{0, 0, 0}, {1, 0, 0}, {1, 0, 0}};
		BAEvolutionModel ba(3,2);
		std::vector<EvolutionModel*> evolution_model = {&ba, &ba, &ba};
		evolve(mnet,num_of_steps,num_of_actors,pr_no_event,pr_internal_event,dependency,evolution_model);
		//log(mnet->to_string());
		//log("Edge Jaccard similarity 1-2: " + to_string(jaccard_similarity(mnet,layer1,layer2)));
		//log("Edge Jaccard similarity 1-3: " + to_string(jaccard_similarity(mnet,layer1,layer3)));
		//log("Edge Jaccard similarity 2-3: " + to_string(jaccard_similarity(mnet,layer2,layer3)));
		//log("Assortativity 1-2: " + to_string(assortativity(mnet,layer1,layer2,OUT)));
		//log("Assortativity 1-3: " + to_string(assortativity(mnet,layer1,layer3,OUT)));
		//log("Assortativity 2-3: " + to_string(assortativity(mnet,layer2,layer3,OUT)));
	}
}

