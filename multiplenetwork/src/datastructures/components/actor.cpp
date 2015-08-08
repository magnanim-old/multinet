#include "datastructures.h"

namespace mlnet {

actor::actor(const actor_id& id, const std::string& name) : named_component(id,name) {}

std::string actor::to_string() const {
	return "Actor: " + named_component::to_string();
}

}
