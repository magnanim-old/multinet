#include "datastructures.h"

namespace mlnet {

edge::edge(const edge_id& id, const NodeSharedPtr& v1, const NodeSharedPtr& v2, bool directed) :
	basic_component(id),
	v1(v1),
	v2(v2),
	directed(directed) {}

std::string edge::to_string() const {
	if (directed)
		return "Edge: " + v1->to_string() + " -> " + v2->to_string();
	else
		return "Edge: " + v1->to_string() + " -- " + v2->to_string();
}

}
