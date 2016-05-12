#include "datastructures.h"

namespace mlnet {

basic_component::basic_component(object_id id) : id(id) {}

bool basic_component::operator==(const basic_component& comp) const {
    return id==comp.id;
}

bool basic_component::operator!=(const basic_component& comp) const {
    return id!=comp.id;
}

bool basic_component::operator<(const basic_component& comp) const {
    return id<comp.id;
}

bool basic_component::operator>(const basic_component& comp) const {
    return id>comp.id;
}

std::string basic_component::to_string() const {
	return "id(" + std::to_string(id) + ")";
}

named_component::named_component(object_id id, const std::string& name) :
		basic_component(id),
		name(name) {}

std::string named_component::to_string() const {
	return "id(" + std::to_string(id) + "), name(" + name + ")";
}

std::ostream& operator<<(std::ostream& os, const named_component& dt) {
	os << dt.name;
	return os;
}


}
