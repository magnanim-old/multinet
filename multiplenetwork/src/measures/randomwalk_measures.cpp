/*
 * degree.cpp
 *
 * Created on: Feb 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "measures.h"
#include "randomwalks.h"
#include "utils.h"
#include <vector>
#include <iostream>

namespace mlnet {

std::unordered_map<ActorSharedPtr, int > occupation(const MLNetworkSharedPtr& mnet, double teleportation, matrix<double> transitions, int num_steps) {
	Walker rw(mnet, teleportation,	transitions);

	std::unordered_map<ActorSharedPtr, int > occupation_map;

	NodeSharedPtr node = rw.now();

	while (num_steps--) {
		node = rw.next();
		if (!rw.action()) {
			continue;
		}
		if (rw.teleported()) {
			// new starting point - not counted
		}
		else {
			if (occupation_map.count(node->actor)==0)
				occupation_map[node->actor] = 0;
			occupation_map[node->actor]++;
		}
	}
	return occupation_map;
}

} // Namespace mlnet

