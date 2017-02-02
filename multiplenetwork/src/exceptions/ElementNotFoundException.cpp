#include "exceptions.h"


namespace mlnet {

ElementNotFoundException::ElementNotFoundException(std::string value) {
	ElementNotFoundException::value = "Not found: " + value;
}

ElementNotFoundException::~ElementNotFoundException() throw () {}

const char* ElementNotFoundException::what() const throw() {
    return value.data();
}

}
