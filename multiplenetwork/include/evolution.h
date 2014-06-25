/**
 * This module defines a generic network co-evolution process.
 *
 * The function "evolve" takes a multiplex network as input and at every step
 * updates each of its networks taking one of the following actions:
 * 1) no action (the network remains unchanged - used to set different evolution speeds)
 * 2) internal evolution (the network evolves according to some internal dynamics)
 * 3) external evolution (the network imports vertexes and edges from another)
 *
 *
 */

#ifndef EVOLUTION_H_
#define EVOLUTION_H_

#include <set>
#include <vector>
#include "evolution.h"
#include "datastructures.h"
#include "utils.h"

/**********************************************************************/
/** Evolution models **************************************************/
/**********************************************************************/
class EvolutionModel {
public:
	virtual void evolution_step(MultiplexNetwork& mnet, network_id net) = 0;
	virtual void init_step(MultiplexNetwork& mnet, network_id net) = 0;
protected:
	Random rand;
};

/**
 * @brief Grows a network by first creating a complete graph with m0 vertexes, then adding a new vertex at a time and connecting it to m other vertexes chosen with a probability proportional to their degree.
 **/
class BAEvolutionModel : public EvolutionModel {
	int m0, m;
public:
	BAEvolutionModel(int m0, int m);
	~BAEvolutionModel();
	void init_step(MultiplexNetwork& mnet, network_id net);
	void evolution_step(MultiplexNetwork& mnet, network_id net);
};

/**
 * @brief Grows a network by first creating all the vertexes and then at every step choosing two (uniform probabilty) to connect with an edge.
 **/
class RandomEvolutionModel : public EvolutionModel {
	int m0;
public:
	RandomEvolutionModel(int m0);
	~RandomEvolutionModel();
	void evolution_step(MultiplexNetwork& mnet, network_id net);
	void init_step(MultiplexNetwork& mnet, network_id net);
};

/**********************************************************************/
/** Evolution method **************************************************/
/**********************************************************************/
/**
 * @brief Grows the input multiplex network.
 * @param mnet MultiplexNetwork to grow
 * @param num_of_steps number of evolution steps
 * @param pr_no_event[] for each network, the probability that an evolution step does not change the network
 * @param pr_internal_event[] for each network, the probability that if something happens this is an internal evolution according to the evolution_model[] parameter
 * @param dependency[][] The (i,j) element of this matrix indicates the probability that, given an external evolution event, network i will consider network j as a potential candidate to import edges from
 * @param evolution_model[] for each network, a specification of how the network should evolve when an internal event happens
 **/
void evolve(MultiplexNetwork &mnet,
		long num_of_steps,
		double pr_no_event[],
		double pr_internal_event[],
		std::vector<std::vector<double> > dependency,
		std::vector<EvolutionModel*> evolution_model);

#endif /* EVOLUTION_H_ */
