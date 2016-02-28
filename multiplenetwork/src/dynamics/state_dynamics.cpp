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

transition::~transition() {}

matrix<long> run(MLNetworkSharedPtr& mnet, const std::vector<std::string>& statuses, const std::vector<double>& init_distribution, const std::string& seed, const std::vector<transition*>& transitions, long num_iterations) {
	// matrix to store number of actors in each status at each timestamp
	matrix<long> result(statuses.size(),std::vector<long>(num_iterations+1));

	// Create status attributes: _S_current, _S_next, _S_time
	mnet->actor_features()->add(_S_current,STRING_TYPE);
	mnet->actor_features()->add(_S_next,STRING_TYPE);
	mnet->actor_features()->add(_S_time,NUMERIC_TYPE);

	// Initialize actor statuses
	Counter<std::string> c;
	for (ActorSharedPtr actor: mnet->get_actors()) {
		std::string status = statuses.at(test(init_distribution));
		c.inc(status);
		mnet->actor_features()->setString(actor->id,_S_current,status);
		mnet->actor_features()->setString(actor->id,_S_next,status);
		mnet->actor_features()->setNumeric(actor->id,_S_time,0);
		mnet->actor_features()->setNumeric(actor->id,_S_time,0);
	}
	for (int i=0; i<statuses.size(); i++) {
		result[i][0] = c.count(statuses.at(i));
	}

	// If a seed has been specified, assign it to a random node
	if (seed!="") {
		mnet->actor_features()->setString(mnet->get_actors().get_at_random()->id,_S_current,seed);
		mnet->actor_features()->setString(mnet->get_actors().get_at_random()->id,_S_next,seed);
	}

	// LOOP
	for (long ts=1; ts<=num_iterations; ts++) {
		Counter<std::string> c;
		for (NodeSharedPtr node: mnet->get_nodes()) {
			if (mnet->actor_features()->getNumeric(node->actor->id,_S_time)==ts) continue;
			for (transition* t: transitions) {
				std::string new_status = t->fire(mnet,node,ts);
				if (!new_status.empty()) {
					mnet->actor_features()->setString(node->actor->id,_S_next,new_status);
					mnet->actor_features()->setNumeric(node->actor->id,_S_time,ts);
				}
			}
		}
		for (ActorSharedPtr actor: mnet->get_actors()) {
			std::string next = mnet->actor_features()->getString(actor->id,_S_next);
			mnet->actor_features()->setString(actor->id,_S_current,next);
			c.inc(next);
		}
		for (int i=0; i<statuses.size(); i++) {
			result[i][ts] = c.count(statuses.at(i));
		}
	}
	return result;
}


}
