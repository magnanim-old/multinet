/*
 * edge.cpp
 *
 * Created on: Feb 27, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "datastructures.h"
#include <algorithm>

edge_id::edge_id(vertex_id v1, vertex_id v2, bool directed) {
	edge_id::v1 = v1;
	edge_id::v2 = v2;
	edge_id::directed = directed;
}

bool edge_id::operator==(const edge_id& e2) const {
    return (directed==e2.directed) &&
    		(((v1==e2.v1)&&(v2==e2.v2)) ||
    		(!directed && (v1==e2.v2) && (v2==e2.v1)));
}

bool edge_id::operator!=(const edge_id& e2) const {
    return ! operator==(e2);
}

bool edge_id::operator<(const edge_id& e2) const {
	if (directed) {
		if (v1<e2.v1) return true;
		if (v1==e2.v1) {
			if (v2<e2.v2) return true;
		}
		return false;
	}
	else {
		if (std::min(v1,v2)<std::min(e2.v1,e2.v2)) return true;
		if (std::min(v1,v2)==std::min(e2.v1,e2.v2)) {
			if (std::max(v1,v2)<std::max(e2.v1,e2.v2)) return true;
		}
		return false;
	}
}

std::ostream& operator<<(std::ostream &strm, const edge_id& eid) {
	strm << eid.to_string();
	return strm;
}

std::string edge_id::to_string() const {
	return "(" + std::to_string(v1) + "," + std::to_string(v2) + ")";
}
