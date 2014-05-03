#include "datastructures.h"

/*
 * interlayer_edge_id.cpp
 *
 * Created on: Feb 27, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "datastructures.h"

interlayer_edge_id::interlayer_edge_id(global_vertex_id v1, global_vertex_id v2, bool directed) {
	interlayer_edge_id::v1 = v1;
	interlayer_edge_id::v2 = v2;
	interlayer_edge_id::directed = directed;
}

bool interlayer_edge_id::operator==(const interlayer_edge_id& e2) const {
    return (directed==e2.directed) &&
    		(((v1==e2.v1)&&(v2==e2.v2)) ||
    		(!directed && (v1==e2.v2) && (v2==e2.v1)));
}

bool interlayer_edge_id::operator!=(const interlayer_edge_id& e2) const {
    return ! operator==(e2);
}

bool interlayer_edge_id::operator<(const interlayer_edge_id& e2) const {
    if (v1<e2.v1) return true;
    if (v1==e2.v1) {
    	if (v2<e2.v2) return true;
    }
    return false;
}

