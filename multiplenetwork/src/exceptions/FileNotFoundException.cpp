#include "exceptions.h"

namespace mlnet {

FileNotFoundException::FileNotFoundException(std::string value) {
	FileNotFoundException::value = value;
}

FileNotFoundException::~FileNotFoundException() throw() {}

const char* FileNotFoundException::what() const throw() {
    return ("File not found: " + value).data();
}

}
