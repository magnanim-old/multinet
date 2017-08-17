/*
 * degree.cpp
 *
 * Created on: Feb 28, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "measures.h"
#include "utils.h"
#include <vector>
#include <iostream>

namespace mlnet {

double cc(const MLNetworkSharedPtr& mnet, const NodeSharedPtr& node) {
	int num_edges = 0;
    NodeListSharedPtr neigh = mnet->neighbors(node,INOUT);
    int num_neigh = neigh->size();
    if (num_neigh<=1) return 0.0;
    
	for (NodeSharedPtr n1: *neigh) {
		for (NodeSharedPtr n2: *neigh) {
            if (n1>=n2) continue;
            if (mnet->get_edge(n1,n2))
				num_edges++;
		}
	}
	return num_edges*2.0/(num_neigh*(num_neigh-1));
}
} // Namespace mlnet

