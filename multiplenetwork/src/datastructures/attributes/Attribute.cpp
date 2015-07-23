#include "datastructures.h"

namespace mlnet {

Attribute::Attribute(const std::string& name, attribute_type type) : aname(name), atype(type) {}

const std::string& Attribute::name() const {
	return aname;
}

int Attribute::type() const {
	return atype;
}

std::string Attribute::type_as_string() const {
	switch (atype) {
	case NUMERIC_TYPE:
		return "NUMERIC";
	case STRING_TYPE:
		return "STRING";
	}
}

}

