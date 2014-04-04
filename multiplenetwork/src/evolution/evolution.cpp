/*
 * evolution.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: magnanim
 */

#include <stdio.h>
#include <vector>
#include <iostream>
#include "evolution.h"

void evolve(MultipleNetwork &mnet,
		long num_of_steps,
		double pr_no_event[],
		double pr_internal_event[],
		std::vector<std::vector<double> > dependency,
		std::vector<EvolutionModel*> evolution_model) {

	Random rand;

		// check size
		for (int i=0; i<mnet.getNumNetworks(); i++) {
			evolution_model[i]->init_step(mnet, i);
		}

		for (long i=0; i<num_of_steps; i++) {
			std::cout << "Step " << i << std::endl;
			for (int n=0; n<mnet.getNumNetworks(); n++) {
				std::cout << "Network " << n << ": ";
				if (rand.test(pr_no_event[n])) {
					std::cout << "no event" << std::endl;
					continue;
				}
				if (rand.test(pr_internal_event[n])) {
					std::cout << "internal event" << std::endl;
					evolution_model[n]->evolution_step(mnet, n);
					continue;
				}
				else {
					std::cout << "external event" << std::endl;
				}
			}
		}
}


