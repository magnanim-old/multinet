#include "datastructures.h"

namespace mlnet {

node::node(node_id id, const ActorSharedPtr& actor, const LayerSharedPtr& layer) :
	basic_component(id),
	actor(actor),
	layer(layer) {}

std::string node::to_string() const {
	return "Node: " + basic_component::to_string() + " as " + actor->to_string() + " on " + layer->to_string();
}

}
