#include "exceptions.h"

FileNotFoundException::FileNotFoundException(std::string path) {
	FileNotFoundException::path = path;
}

FileNotFoundException::~FileNotFoundException() throw () {}

const char* FileNotFoundException::what() const throw() {
    return (std::string("File not found: ") + path).data();
}
