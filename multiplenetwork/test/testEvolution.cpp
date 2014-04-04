
#include "test.h"
#include "multiplenetwork.h"
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void testEvolution() {

	cout << "Creating a multiple network with two empty undirected networks...";
	MultipleNetwork mnet;
	mnet.addVertexes(100);
	Network n1, n2;
	mnet.addNetwork(n1);
	mnet.addNetwork(n2);
	cout << "done!" << endl;

	long num_of_steps = 100;
	double pr_no_event[] = {0.0,0.5};
	double pr_internal_event[] = {1.0,0.0};
	vector<vector<double> > dependency;
	vector<double> d1;
	d1.push_back(0.0);
	d1.push_back(1.0);
	vector<double> d2;
	d2.push_back(1.0);
	d2.push_back(0.0);
	dependency.push_back(d1);
	dependency.push_back(d2);

	//double gdependency[2][2] = {{0.0,1.0},{1.0,0.0}};

	BAEvolutionModel ba(4); // PARAMETERS

	std::vector<EvolutionModel*> evolution_model;
	evolution_model.push_back(&ba); // check if by reference
	evolution_model.push_back(&ba);

	evolve(mnet,
			num_of_steps,
			pr_no_event,
			pr_internal_event,
			dependency,
			evolution_model);

	print(mnet);
}
