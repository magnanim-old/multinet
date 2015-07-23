#include "datastructures.h"

namespace mlnet {

layer::layer(const layer_id& id, const std::string& name) :
	id(id),
	name(name) {}

layer::~layer() {}

bool layer::operator==(const layer& l) const {
    return id==l.id;
}

bool layer::operator!=(const layer& l) const {
    return id!=l.id;
}

bool layer::operator<(const layer& l) const {
    return id<l.id;
}

bool layer::operator>(const layer& l) const {
    return id>l.id;
}

std::string layer::to_string() const {
	return "L[" + name + "]";
}

}
