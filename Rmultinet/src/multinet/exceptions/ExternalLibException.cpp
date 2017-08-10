#include "exceptions.h"


namespace mlnet {
    
    ExternalLibException::ExternalLibException(std::string value) {
        ExternalLibException::value = "Problem in a call to external library: " + value;
    }
    
    ExternalLibException::~ExternalLibException() throw () {}
    
    const char* ExternalLibException::what() const throw() {
        return value.data();
    }
    
}
