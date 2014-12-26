#include "datastructures.h"
#include <sstream>
#include <algorithm>

/* In this implementation the order of v1 and v2 may not correspond to
 * the order in which they are passed to the constructor when the edge
 * is undirected. */
edge_id::edge_id(vertex_id v1, vertex_id v2, bool directed) :
	v1(directed?v1:std::min(v1,v2)),
	v2(directed?v2:std::max(v1,v2)),
	directed(directed) {}

bool edge_id::operator==(const edge_id& e2) const {
    return (directed==e2.directed) && (v1==e2.v1) && (v2==e2.v2);
}

bool edge_id::operator!=(const edge_id& e2) const {
    return ! operator==(e2);
}

bool edge_id::operator<(const edge_id& e2) const {
	if (directed<e2.directed) return true;
	else if (directed>e2.directed) return false;

	if (v1<e2.v1) return true;
	else if (v1>e2.v1) return false;

	if (v2<e2.v2) return true;
	else if (v1>e2.v1) return false;

    return false;
}

bool edge_id::operator>(const edge_id& e) const {
    return ! operator<(e) && ! operator==(e);
}

std::ostream& operator<<(std::ostream &strm, const edge_id& eid) {
	if (eid.directed) strm << "(" << eid.v1 << "," << eid.v2 << ")";
	else strm << "{" << eid.v1 << "," << eid.v2 << "}";
	return strm;
}

std::string edge_id::to_string() const {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}
