/*
 * A class implementing a community detection algorithms on single layer network
 * This implementation is
 */

#ifndef SINGLELAYER_H_
#define SINGLELAYER_H_

#include "walktrap.h"
#include "datastructures.h"
#include "community.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <random>

/***********************************/
/**                  **/
/***********************************/
namespace mlnet {

/**
 *
 */
class SingleLayer {
private:


public:

	static CommunitiesSharedPtr walkTrap(const MLNetworkSharedPtr&, const LayerSharedPtr&);
	~SingleLayer();
};

}

#endif /* SINGLELAYER_H_ */
