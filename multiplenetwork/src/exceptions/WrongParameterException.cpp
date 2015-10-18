#include "exceptions.h"

#include <sstream>

using namespace std;

WrongParameterException::WrongParameterException(string value) {
	WrongParameterException::value = "Wrong parameter: " + value;
}
WrongParameterException::~WrongParameterException() throw (){
	// TODO Auto-generated destructor stub
}
const char* WrongParameterException::what() const throw()
{
    return value.data();
}
