/*
 * A class implementing a random walker moving across a multilayer network.
 * This implementation is
 */

#ifndef MLNET_RANDOMWALKS_H_
#define MLNET_RANDOMWALKS_H_

#include "datastructures.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <random>

/***********************************/
/** Random walks                  **/
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
	/**
	 * Creation of a new walker.
	 * @param mlnet the multilayer network where the walker will be active
	 * @param teleportation the probability that the walker jumps on a random node
	 * @param transitions a matrix where {i,j} indicates the probability that the
	 * walker jumps from the i^th layer to the j^th layer. {i,i} is the probability that
	 * the walker remains on the same layer.
	 */
	Walker(const MLNetworkSharedPtr mlnet, double teleportation, const matrix<double>& transitions);
	~Walker();
	/**
	 * Returns the current position of the walker without moving it
	 * @return the current node where the walker is
	 */
	NodeSharedPtr now();
	/**
	 * Moves the walker. The walker may also remain on the same node, for example if there are no neighbors
	 * and there is no teleportation, in which case the same node is returned and a call to action() would return false.
	 * @return the new node where the walker is, after a step
	 */
	NodeSharedPtr next();
	/**
	 * Checks the nature of the last move
	 * @return the next node visited by the walker is returned.
	 */
	bool teleported();
	/**
	 * Checks the nature of the last move. If the walker gets stuck on a node without neighbors,
	 * it will return no action until when it is teleported.
	 * @return true if the last move was an actual move.
	 */
	bool action();
};

}

#endif /* MLNET_RANDOMWALKS_H_ */
