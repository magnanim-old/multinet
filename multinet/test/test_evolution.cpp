
#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <iostream>
#include <sstream>
#include <string>
#include "../include/multinet.h"

using namespace mlnet;

void test_evolution() {
	{
	test_begin("Evolution");
	std::cout << "[Evolution Test R1] N1: larger independent, N2: smaller independent and with basic random graph evolution model" << std::endl;
	// set evolution parameters
	long num_of_steps = 100;
	long num_of_actors = 1000;
	std::vector<double> pr_internal_event = {0.0,0.3};
	std::vector<double> pr_external_event = {1.0,0.7};
	matrix<double> dependency = {{0, 1}, {1, 0}};
	EvolutionModelSharedPtr ba(new BAEvolutionModel(3,2));
	EvolutionModelSharedPtr ra(new UniformEvolutionModel(70));
	std::vector<EvolutionModelSharedPtr> evolution_model = {ba, ra};
	evolve(num_of_steps,num_of_actors,pr_internal_event,pr_external_event,dependency,evolution_model);
	}

	{
	std::cout << "[Evolution Test R2] N1: larger independent, N2: smaller dependent on N1, both with basic random graph evolution model" << std::endl;
	// set evolution parameters
	long num_of_steps = 100;
	long num_of_actors = 1000;
	std::vector<double> pr_internal_event = {0.0,0.3};
	std::vector<double> pr_external_event = {1.0,0.7};
	matrix<double> dependency = {{0, 1}, {1, 0}};
	EvolutionModelSharedPtr ra(new UniformEvolutionModel(30));
	std::vector<EvolutionModelSharedPtr> evolution_model = {ra, ra};
	evolve(num_of_steps,num_of_actors,pr_internal_event,pr_external_event,dependency,evolution_model);
	}
    {
	std::cout << "[Evolution Test 5] LARGER NETWORKS, COMMON ACTORS - N1: larger independent, N2: smaller dependent more on N1 than on N3, N3: smaller almost independent" << std::endl;
		// set evolution parameters
		long num_of_steps = 1000;
		long num_of_actors = 10000;
		std::vector<double> pr_internal_event = {0.0,0.3,0.3};
		std::vector<double> pr_external_event = {1.0,0.7,0.7};
		matrix<double> dependency = {{0, 1, 0}, {1, 0, 0}, {1, 0, 0}};
		EvolutionModelSharedPtr ba(new BAEvolutionModel(3,2));
		std::vector<EvolutionModelSharedPtr> evolution_model = {ba, ba, ba};
		evolve(num_of_steps,num_of_actors,pr_internal_event,pr_external_event,dependency,evolution_model);
	}
	test_end("Evolution");
}

