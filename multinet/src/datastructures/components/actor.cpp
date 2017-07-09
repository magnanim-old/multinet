#include "datastructures.h"

namespace mlnet {

actor::actor(actor_id id, const std::string& name) : named_component(id,name) {}

std::string actor::to_string() const {
	return name;
}

}
