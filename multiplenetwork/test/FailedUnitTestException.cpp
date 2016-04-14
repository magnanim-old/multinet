#include "test.h"

FailedUnitTestException::FailedUnitTestException(std::string message) {
	FailedUnitTestException::message = message;
}

FailedUnitTestException::~FailedUnitTestException() throw () {}

const char* FailedUnitTestException::what() const throw() {
    return (std::string("[FAIL] ") + message).data();
}
