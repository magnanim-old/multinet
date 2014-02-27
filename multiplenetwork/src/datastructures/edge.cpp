/*
 * edge.cpp
 *
 * Created on: Feb 27, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "datastructures.h"

edge::edge(vertex_id v1, vertex_id v2, network_id network) {
	edge::v1 = v1;
	edge::v2 = v2;
	edge::network = network;
}

bool edge::operator==(const edge& e2) const {
    return (v1==e2.v1)&&(v2==e2.v2)&&(network==e2.network);
}

bool edge::operator<=(const edge& e2) const {
    if (v1<e2.v1) return true;
    if (v1==e2.v1) {
    	if (v2<e2.v2) return true;
    	if (v2==e2.v2) {
    		if (network<=e2.network) return true;
    	}
    }
    return false;
}

bool edge::operator<(const edge& e2) const {
    if (v1<e2.v1) return true;
    if (v1==e2.v1) {
    	if (v2<e2.v2) return true;
    	if (v2==e2.v2) {
    		if (network<e2.network) return true;
    	}
    }
    return false;
}

