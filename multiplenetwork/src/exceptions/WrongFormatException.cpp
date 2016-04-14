#include "exceptions.h"

WrongFormatException::WrongFormatException(std::string value) {
	WrongFormatException::value = "Format error: " + value;
}

WrongFormatException::~WrongFormatException() throw () {}

const char* WrongFormatException::what() const throw() {
    return value.data();
}
