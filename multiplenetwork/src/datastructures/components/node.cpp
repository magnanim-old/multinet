#include "datastructures.h"

namespace mlnet {

node::node(const node_id& id, const std::string& name, const ActorSharedPtr& actor, const LayerSharedPtr& layer) :
	id(id),
	name(name),
	actor(actor),
	layer(layer) {}

node::~node() {}

bool node::operator==(const node& e) const {
    return id==e.id;
}

bool node::operator!=(const node& e) const {
    return id!=e.id;
}

bool node::operator<(const node& e) const {
    return id<e.id;
}

bool node::operator>(const node& e) const {
    return id>e.id;
}

std::string node::to_string() const {
	return "N[" + name + "," + layer->to_string() + "]";
}

}
