/*
 * edge.cpp
 *
 * Created on: Feb 27, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "datastructures.h"

edge_id::edge_id(vertex_id v1, vertex_id v2, bool directed) {
	edge_id::v1 = v1;
	edge_id::v2 = v2;
	edge_id::directed = directed;
}

bool edge_id::operator==(const edge_id& e2) const {
    return ((v1==e2.v1)&&(v2==e2.v2)) ||
    		(!directed && (v1==e2.v2) && (v2==e2.v1));
}

/*
bool edge_id::operator<=(const edge_id& e2) const {
    if (v1<e2.v1) return true;
    if (v1==e2.v1) {
    	if (v2<e2.v2) return true;
    	if (v2==e2.v2) {
    		if (network<=e2.network) return true;
    	}
    }
    return false;
}
*/

bool edge_id::operator<(const edge_id& e2) const {
    if (v1<e2.v1) return true;
    if (v1==e2.v1) {
    	if (v2<e2.v2) return true;
    }
    return false;
}

