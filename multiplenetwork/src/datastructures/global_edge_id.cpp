#include "datastructures.h"
#include <algorithm>

global_edge_id::global_edge_id(vertex_id v1, vertex_id v2, network_id network, bool directed) :
	v1(directed?v1:std::min(v1,v2)),
	v2(directed?v2:std::max(v1,v2)),
	network(network),
	directed(directed) {}

bool global_edge_id::operator==(const global_edge_id& e2) const {
    return (directed==e2.directed) && (network==e2.network) && (v1==e2.v1) && (v2==e2.v2);
}

bool global_edge_id::operator!=(const global_edge_id& e2) const {
    return !(*this == e2);
}

bool global_edge_id::operator<(const global_edge_id& e2) const {
	if (directed<e2.directed) return true;
	else if (directed>e2.directed) return false;

	if (v1<e2.v1) return true;
	else if (v1>e2.v1) return false;

	if (v2<e2.v2) return true;
	else if (v1>e2.v1) return false;

	if (network<e2.network) return true;
	else if (network>e2.network) return false;

    return false;
}


bool global_edge_id::operator>(const global_edge_id& e) const {
    return ! operator<(e) && ! operator==(e);
}


std::ostream& operator<<(std::ostream &strm, const global_edge_id& eid) {
	if (eid.directed) strm << "(" << eid.v1 << "," << eid.v2 << ",N:" << eid.network << ")";
	else strm << "{" << eid.v1 << "," << eid.v2 << ",N:" << eid.network << "}";
	return strm;
}

std::string global_edge_id::to_string() const {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}
