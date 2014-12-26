#include "datastructures.h"
#include <sstream>

inter_edge_id::inter_edge_id(global_vertex_id v1, global_vertex_id v2, bool directed) :
	v1(directed?v1:std::min(v1,v2)),
	v2(directed?v2:std::max(v1,v2)),
	directed(directed) {}

bool inter_edge_id::operator==(const inter_edge_id& e2) const {
    return (directed==e2.directed) && (v1==e2.v1) && (v2==e2.v2);
}

bool inter_edge_id::operator!=(const inter_edge_id& e2) const {
    return ! operator==(e2);
}

bool inter_edge_id::operator<(const inter_edge_id& e2) const {
	if (directed<e2.directed) return true;
	else if (directed>e2.directed) return false;

	if (v1<e2.v1) return true;
	else if (v1>e2.v1) return false;

	if (v2<e2.v2) return true;
	else if (v1>e2.v1) return false;

    return false;
}

bool inter_edge_id::operator>(const inter_edge_id& e) const {
    return ! operator<(e) && ! operator==(e);
}

std::ostream& operator<<(std::ostream &strm, const inter_edge_id& eid) {
	strm << "(" << eid.v1 << "," << eid.v2 << ")";
	return strm;
}

std::string inter_edge_id::to_string() const {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}
