#include "datastructures.h"

namespace mlnet {

layer::layer(layer_id id, const std::string& name) : named_component(id,name) {}

std::string layer::to_string() const {
	return "Layer: " + named_component::to_string();
}

}
