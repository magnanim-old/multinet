/*
 * ElementNotFoundException.cpp
 *
 *  Created on: Jul 1, 2013
 *  Author: Matteo Magnani <matteo.magnani@it.uu.se>
 */

#include "exceptions.h"

#include <sstream>

using namespace std;

WrongFormatException::WrongFormatException(string value) {
	WrongFormatException::value = "Format error: " + value;
}
WrongFormatException::~WrongFormatException() throw (){
	// TODO Auto-generated destructor stub
}
const char* WrongFormatException::what() const throw()
{
    return value.data();
}
