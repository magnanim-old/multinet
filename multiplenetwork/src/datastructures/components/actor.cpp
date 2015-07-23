#include "datastructures.h"

namespace mlnet {

actor::actor(actor_id id, const std::string& name) :
	id(id),
	name(name) {}

actor::~actor() {}

bool actor::operator==(const actor& a) const {
    return id==a.id;
}

bool actor::operator!=(const actor& a) const {
    return id!=a.id;
}

bool actor::operator<(const actor& a) const {
    return id<a.id;
}

bool actor::operator>(const actor& a) const {
    return id>a.id;
}

std::string actor::to_string() const {
	return "@[" + std::to_string(id) + "]";
}

}
