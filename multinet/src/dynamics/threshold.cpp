/*
 * contagion.cpp
 *
 * Created on: Feb 27, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "dynamics.h"
#include <unordered_map>
#include <iostream>

namespace mlnet {
/* Work in progress
 
threshold_transition::threshold_transition(const std::string& status1, const std::string& status2, double th) :
		status1(status1), status2(status2), th(th) {
}

threshold_transition::~threshold_transition() {}

std::string threshold_transition::fire(MLNetworkSharedPtr& mnet, const NodeSharedPtr& node, long time) {
	std::string current_status = mnet->actor_features()->getString(node->actor->id,_S_current);
	std::string new_status = (current_status==status1)?status2:status1;
	long num_neighbors = 0;
	long other_status = 0;
	for (NodeSharedPtr n: *mnet->neighbors(node,IN)) {
		num_neighbors++;
		if (mnet->actor_features()->getString(n->actor->id,_S_current)==new_status) {
			other_status++;
		}
	}
	if (other_status/num_neighbors>=th)
		return new_status;
	else return "";
}


matrix<long> threshold(MLNetworkSharedPtr& mnet, double perc_adopters, double th, long num_iterations) {
	std::vector<std::string> statuses = {"A","B"};
	std::vector<double> init_distribution({1-perc_adopters,perc_adopters});
	std::string seed = "";
	threshold_transition AB("A","B",th);
	std::vector<transition*> transitions = {&AB};

	return run(mnet, statuses, init_distribution, seed, transitions, num_iterations);
}
*/
}
