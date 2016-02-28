/*
 * measures.h
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: beta
 *
 * Social Network Analysis measures for multiplex networks.
 * TESTED:
 * Node based: degree, neighborhood, exclusive neighborhood, network relevance, exclusive network relevance
 * Network based: similarity
 *
 * TO BE CONSOLIDATED:
 * Distance based: Pareto distance, Pareto betweenness
 * Group based: modularity
 *
 * References to be added.
 */

#ifndef MULTIPLENETWORK_DYNAMICS_H_
#define MULTIPLENETWORK_DYNAMICS_H_

#include <exception>
#include <vector>
#include <string>
#include "datastructures.h"

namespace mlnet {

/**********************************************************************/
/** Contageous spreading **********************************************/
/**********************************************************************/

// Name of variables
const static std::string _S_current = "_S_current";
const static std::string _S_next = "_S_next";
const static std::string _S_time = "_S_time";

class transition {
public:
	virtual ~transition() = 0;
	virtual std::string fire(MLNetworkSharedPtr& mnet, const NodeSharedPtr& node, long time) = 0;
};

class beta_transition : public transition {
private:
	std::string neighbor_status;
	std::string status;
	double beta;
	std::string new_status;
public:
	beta_transition(const std::string& neighbor_status, const std::string& status, double beta, const std::string& new_status);
	~beta_transition();
	std::string fire(MLNetworkSharedPtr& mnet, const NodeSharedPtr& node, long time);
};

class tau_transition : public transition {
private:
	std::string status;
	double tau;
	std::string new_status;
public:
	tau_transition(const std::string& status, int tau, const std::string& new_status);
	~tau_transition();
	std::string fire(MLNetworkSharedPtr& mnet, const NodeSharedPtr& node, long time);
};

class threshold_transition : public transition {
private:
	std::string status1;
	std::string status2;
	double th;
public:
	threshold_transition(const std::string& status1, const std::string& status2, double th);
	~threshold_transition();
	std::string fire(MLNetworkSharedPtr& mnet, const NodeSharedPtr& node, long time);
};

matrix<long> run(MLNetworkSharedPtr& mnet, const std::vector<std::string>& statuses, const std::vector<double>& init_distribution, const std::string& seed, const std::vector<transition*>& transitions, long num_iterations);

matrix<long> sir(MLNetworkSharedPtr& mnet, double beta, int tau, long num_iterations);
matrix<long> threshold(MLNetworkSharedPtr& mnet, double perc_adopters, double th, long num_iterations);

} // namespace mlnet

#endif /* MULTIPLENETWORK_DYNAMICS_H_ */
