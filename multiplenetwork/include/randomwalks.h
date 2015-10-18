/*
 * utils.h
 *
 * Contains:
 * - logging functions
 * - basic IO (csv file reading)
 * - random functions
 */

#ifndef MULTIPLENETWORK_RANDOMWALKS_H_
#define MULTIPLENETWORK_RANDOMWALKS_H_

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <random>
#include "datastructures.h"

/***********************************/
/** Random                        **/
/***********************************/
namespace mlnet {

/**
 *
 */
class Walker {
private:
	MLNetworkSharedPtr mlnet;
	NodeSharedPtr current;
	double teleportation;
	matrix<double> transitions;
	bool just_teleported;
	bool no_action;
	std::unordered_map<layer_id, int> layer_idx;

public:
	Walker(const MLNetworkSharedPtr mlnet, double teleportation, const matrix<double>& transition_prob);
	~Walker();

	NodeSharedPtr now();
	NodeSharedPtr next();
	bool teleported();
	bool action();
};

}

#endif /* MULTIPLENETWORK_UTILS_H_ */
