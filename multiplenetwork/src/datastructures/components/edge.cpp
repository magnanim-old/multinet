#include "datastructures.h"

namespace mlnet {

edge::edge(edge_id id, const NodeSharedPtr& v1, const NodeSharedPtr& v2, edge_directionality _directionality) :
	basic_component(id),
	v1(v1),
	v2(v2),
	directionality(_directionality) {}

std::string edge::to_string() const {
	switch (directionality) {
	case DIRECTED: return "Edge: " + v1->to_string() + " -> " + v2->to_string();
	case UNDIRECTED: return "Edge: " + v1->to_string() + " -- " + v2->to_string();
	}
	return ""; // cannot get here
}

}
