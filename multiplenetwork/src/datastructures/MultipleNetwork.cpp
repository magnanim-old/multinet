/**
 * @class MultipleNetwork
 *
 * @author Matteo Magnani <matteo.magnani@it.uu.se>
 * @date August 2013
 * @version 0.1.0
 *
 * Data structure containing several interdependent networks. In this version the
 * only kind of interdependency is node correspondence: every vertex in one of the networks
 * has a global identifier, and different nodes may share the same global identifier.
 *
 */

#include "datastructures.h"
#include "exceptions.h"
#include "utils.h"
#include <iostream>
#include <sstream>

MultipleNetwork::MultipleNetwork() {}

MultipleNetwork::~MultipleNetwork() {
	// TODO
}

void print(MultipleNetwork& mnet) {
	std::cout << "*MULTIPLE NETWORK*\n";
	std::cout << "Number of vertexes: " << mnet.getNumVertexes() << "\n";
	std::cout << "Number of edges: " << mnet.getNumEdges() << "\n";
}

