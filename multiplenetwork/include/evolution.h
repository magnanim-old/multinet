/*
 * evolution.h
 *
 *  Created on: Jun 11, 2013
 *      Author: magnanim
 */

#ifndef EVOLUTION_H_
#define EVOLUTION_H_

#include <set>
#include <vector>
#include "evolution.h"
#include "datastructures.h"
#include "utils.h"

class EvolutionModel {
public:
	virtual void evolution_step(MultipleNetwork& mnet, network_id net) = 0;
	virtual void init_step(MultipleNetwork& mnet, network_id net) = 0;
};

class BAEvolutionModel : public EvolutionModel {
	int num_of_neighbors;
	std::set<global_vertex_id> universe;
public:
	BAEvolutionModel(int m);
	virtual ~BAEvolutionModel();
	void init_step(MultipleNetwork& mnet, network_id net);
	void evolution_step(MultipleNetwork& mnet, network_id net);
};

std::set<long> ba_choice(Network& net, int m);

class RandomEvolutionModel : public EvolutionModel {
	int m0;
	double p;
	Random r;
public:
	RandomEvolutionModel(int m0, double p);
	~RandomEvolutionModel();
	void evolution_step(MultipleNetwork& mnet, network_id net);
	void init_step(MultipleNetwork& mnet, network_id net);
};

void evolve(MultipleNetwork &mnet,
		long num_of_steps,
		double pr_no_event[],
		double pr_internal_event[],
		std::vector<std::vector<double> > dependency,
		//double ** dependency,
		std::vector<EvolutionModel*> evolution_model);

#endif /* EVOLUTION_H_ */

/*template<class fwditer>
fwditer random_unique(fwditer begin, fwditer end, size_t num_random) {
    size_t left = std::distance(begin, end);
    while (num_random--) {
        fwditer r = begin;
        std::advance(r, rand()%left);
        std::swap(*begin, *r);
        ++begin;
        --left;
    }
    return begin;
}*/
