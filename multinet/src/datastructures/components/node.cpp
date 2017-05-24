#include "datastructures.h"

namespace mlnet {

node::node(node_id id, const ActorSharedPtr& actor, const LayerSharedPtr& layer) :
	basic_component(id),
	actor(actor),
	layer(layer) {}

std::string node::to_string() const {
	return actor->to_string() + "::" + layer->to_string();
}

}
