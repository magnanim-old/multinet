#include "exceptions.h"


OperationNotSupportedException::OperationNotSupportedException(std::string value) {
	OperationNotSupportedException::value = "Operation not supported: " + value;
}

OperationNotSupportedException::~OperationNotSupportedException() throw () {}

const char* OperationNotSupportedException::what() const throw() {
    return value.data();
}
