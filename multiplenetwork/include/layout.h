/**
 * This module defines ...
 */

#ifndef LAYOUT_H_
#define LAYOUT_H_

#include <set>
#include <vector>
#include "datastructures.h"
#include "utils.h"


namespace mlnet {

struct coordinates {
public:
	double x;
	double y;
	double z;
};


double fr(double p, double k);
double fain(double p, double k);
double fainter(double p, double k);
hashtable<NodeSharedPtr,coordinates> multiforce(MLNetworkSharedPtr mnet, double width, double length, int interations);
}

#endif /* LAYOUT_H_ */
