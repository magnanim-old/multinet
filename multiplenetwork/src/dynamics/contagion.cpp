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

beta_transition::beta_transition(const std::string& neighbor_status, const std::string& status, double beta, const std::string& new_status) :
		neighbor_status(neighbor_status), status(status), beta(beta), new_status(new_status) {
}

beta_transition::~beta_transition() {}

std::string beta_transition::fire(MLNetworkSharedPtr& mnet, const NodeSharedPtr& node, long time) {
	std::string current_status = mnet->actor_features()->getString(node->actor->id,_S_current);
	if (current_status==status) {
		for (NodeSharedPtr n: *mnet->neighbors(node,IN)) {
			if (mnet->actor_features()->getString(n->actor->id,_S_current)==neighbor_status && test(beta)) {
				return new_status;
			}
		}
	}
	return "";
}

tau_transition::tau_transition(const std::string& status, int tau, const std::string& new_status) :
		status(status), tau(tau), new_status(new_status) {
}

tau_transition::~tau_transition() {}

std::string tau_transition::fire(MLNetworkSharedPtr& mnet, const NodeSharedPtr& node, long time) {
	std::string current_status = mnet->actor_features()->getString(node->actor->id,_S_current);
	if (current_status==status) {
		if (time - mnet->actor_features()->getNumeric(node->actor->id,_S_time)>=tau) {
			return new_status;
		}
	}
	return "";
}


matrix<long> sir(MLNetworkSharedPtr& mnet, double beta, int tau, long num_iterations) {
	std::vector<std::string> statuses = {"S","I","R"};
	std::vector<double> init_distribution({1,0,0});
	std::string seed = "I";
	beta_transition ISI("I","S",beta,"I");
	tau_transition IR("I",tau,"R");
	std::vector<transition*> transitions = {&ISI,&IR};

	return run(mnet, statuses, init_distribution, seed, transitions, num_iterations);
}

}
