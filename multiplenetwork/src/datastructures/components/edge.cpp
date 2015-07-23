#include "datastructures.h"

namespace mlnet {

edge::edge(const edge_id& id, const NodeSharedPtr& v1, const NodeSharedPtr& v2, bool directed) :
	id(id),
	v1(v1),
	v2(v2),
	directed(directed) {}

edge::~edge() {}


bool edge::operator==(const edge& e) const {
    return id==e.id;
}

bool edge::operator!=(const edge& e) const {
	return id!=e.id;
}

bool edge::operator<(const edge& e) const {
	return id<e.id;
}

bool edge::operator>(const edge& e) const {
	return id>e.id;
}

std::string edge::to_string() const {
	if (directed) return "(" + v1->to_string() + " -> " + v2->to_string() + ")";
	else return "E(" + v1->to_string() + " -- " + v2->to_string() + ")";
}


}
